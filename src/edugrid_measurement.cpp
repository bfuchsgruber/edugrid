/************************************************************************
 * @file edugrid_measurement.cpp
 * @date 2023/11/05
 *
 ***********************************************************************/

/************************************************************************
 * Include
 ************************************************************************/
#include <edugrid_measurement.h>

/*************************************************************************
 * Define
 ************************************************************************/

/*************************************************************************
 * Variable Definition
 ************************************************************************/
using namespace ADS1X15;
// ADS1115<TwoWire> ads(Wire);
ADS1015<TwoWire> ads(Wire);

/* Measurements */
float edugrid_measurement::V_in = 0;
float edugrid_measurement::V_out = 0;
float edugrid_measurement::I_in = 0;
float edugrid_measurement::I_out = 0;
/* Measurements last cycle*/
float edugrid_measurement::V_in_lastCycle = 0;
float edugrid_measurement::V_out_lastCycle = 0;
float edugrid_measurement::I_in_lastCycle = 0;
float edugrid_measurement::I_out_lastCycle = 0;
/* Calculations */
float edugrid_measurement::P_in = 0;
float edugrid_measurement::P_out = 0;
uint8_t edugrid_measurement::efficiency = 0;
float edugrid_measurement::dV = 0;
float edugrid_measurement::dI = 0;
float edugrid_measurement::dI_dV_ratio = 0;
float edugrid_measurement::I_V_ratio = 0;

/* ADC */
int16_t edugrid_measurement::ads_raw = 0;
float edugrid_measurement::ads_voltage = 0.0;
float edugrid_measurement::volatage_buffer = 0.0;

/* Encoder */
volatile long edugrid_measurement::encoder_time_SW = 0;
long edugrid_measurement::encoder_old_state = 0;
long edugrid_measurement::encoder_new_state = 0;
ESP32Encoder encoder;

/* Watchdog */
bool edugrid_measurement::watchdog_off_request = false;

/*************************************************************************
 * Function Definition
 ************************************************************************/

/** Limit PWM duty cycle if output voltage is to high
 */
void edugrid_measurement::batteryWatchdog()
{
    if (edugrid_simulation::getSimulationState())
    {
        watchdog_off_request = false;
    }
    else
    {
        if ((V_out >= BATTERY_CUT_OFF_VOLTAGE))
        { // cut PWM
            watchdog_off_request = true;
            edugrid_pwm_control::setPWM(PWM_ABS_MIN, true);
            Serial.println("|WARN| Battery Watchdog active!");
        }

        if ((V_out < BATTERY_CUT_OFF_VOLTAGE))
        { // release PWM again
            watchdog_off_request = false;
        }
    }
}

/** esp32 ADC
 * Measure a voltage with the internal esp32 ADC
 * @param pin GPIO pin where voltage should be measured
 * @return Voltage in unit Volts
 */
float edugrid_measurement::internal_getVoltage(uint8_t pin)
{
    pinMode(pin, INPUT);
    return (analogRead(pin) * ADC_MAX_VOLTAGE / ADC_MAX_RESOLUTION);
}

/** external ADC
 * Init external ADC
 * @param
 * @return
 */
void edugrid_measurement::external_adcInit()
{
    ads.begin(ADS_ADDRESS);
}

/** external ADC
 * Measure a voltage with the external ADC
 * @param pin Hardware pin on ADS1115 where voltage should be measured (A0 --> A3)
 * @param ratio Set the ratio of a hardware voltage divider
 * @return Voltage in unit Volts
 */
float edugrid_measurement::external_getVoltage(uint8_t pin, float ratio)
{
    /* "ratio" is used for calculate a voltage devider */

    if (pin <= ADS_CHANNEL - 1) // pin 0-4
    {
        ads_raw = ads.readADCSingleEnded(pin);
        ads_voltage = ads.computeVolts(ads_raw) * ratio;
        return ads_voltage;
    }
    else
    {
        Serial.println("|FAIL| ADS pin out of range");
        return 0;
    }
}

/** ACS712 current sensor
 * Measure a current with the external ADC and the external HAL sensor
 * @param pin Hardware pin on ADS1115 where current should be measured (A0 --> A3)
 * @param range Set the type of current sensor (5, 20, 30)
 * @param ratio Set the ratio of a hardware voltage divider
 * @return Current in unit Ampere
 */
float edugrid_measurement::external_getCurrent(uint8_t pin, uint8_t range, float ratio)
{
    float retVal = 0.0;
    volatage_buffer = external_getVoltage(pin, ratio);

    switch (range)
    {
    case 5:
        retVal = (volatage_buffer - ACS712_OFFSET) / ACS712_5A_SENSIVITY;
        break;
    case 20:
        retVal = (volatage_buffer - ACS712_OFFSET) / ACS712_20A_SENSIVITY;
        break;
    case 30:
        retVal = (volatage_buffer - ACS712_OFFSET) / ACS712_30A_SENSIVITY;
        break;
    default:
        break;
    }

    retVal += ACS712_CAL_OFFSET;
    retVal *= ACS712_CAL_FACTOR;
    return retVal;
}

/** Get voltage of PV module (= U_IN)
 * (Measured or from simulation)
 * @return Voltage in Volts
 */
float edugrid_measurement::getVoltagePV()
{
    if (edugrid_simulation::getSimulationState())
    {
        return edugrid_simulation::getVoltagePV_simulation();
    }
    else
    {
        return external_getVoltage(PIN_VOLTAGE_IN, DIVIDER_VOLTAGE_IN);
    }
}

/** Get voltage of load (= U_OUT)
 * (Measured or from simulation)
 * @return Voltage in Volts
 */
float edugrid_measurement::getVoltageLoad()
{
    if (edugrid_simulation::getSimulationState())
    {
        return edugrid_simulation::getVoltageLoad_simulation();
    }
    else
    {
        return external_getVoltage(PIN_VOLTAGE_OUT, DIVIDER_VOLTAGE_OUT);
    }
}

/** Get current of PV module (= I_IN)
 * (Measured or from simulation)
 * @return Current in Ampere
 */
float edugrid_measurement::getCurrentPV()
{
    if (edugrid_simulation::getSimulationState())
    {
        return edugrid_simulation::getCurrentPV_simulation();
    }
    else
    {
        return external_getCurrent(PIN_CURRENT_IN, ACS712_RANGE_SET, DIVIDER_CURRENT_IN);
    }
}

/** Get current of load (= I_OUT)
 * (Measured or from simulation)
 * @return Current in Ampere
 */
float edugrid_measurement::getCurrentLoad()
{
    if (edugrid_simulation::getSimulationState())
    {
        return edugrid_simulation::getCurrentLoad_simulation();
    }
    else
    {
        return external_getCurrent(PIN_CURRENT_OUT, ACS712_RANGE_SET, DIVIDER_CURRENT_OUT);
    }
}

/** Load values of all sensors into public variables
 * @return None --> access vars directly!
 */
void edugrid_measurement::getSensors()
{
    /* Get all sensor readings */
    V_in = getVoltagePV();
    I_in = getCurrentPV();
    V_out = getVoltageLoad();
    I_out = getCurrentLoad();

    /* Calculate In Power */
    P_in = V_in * I_in;

    /* Calculate Out Power */
    // #if defined(MISSING_VOLTAGE_DEVIDER_WORKAROUND) && not edugrid_simulation::getSimulationState()
    //     /* WORKAROUND
    //      * As there is now voltage devider at the load side, V_out will estimated by approximate the efficiency of the buck
    //      * Only active, if SIMULATION-define is not set and WORKAROUND-define is set!
    //      */
    //     P_out = (P_in * (float)ETA_BUCK) / 100.;
    //     if (I_out == 0)
    //     {
    //         V_out = 0; // avoid div by zero!
    //     }
    //     else
    //     {
    //         V_out = P_out / I_out; // approx
    //     }
    //     efficiency = ETA_BUCK;
    // #else
    /* NORMAL CASE WITH 4 SENSORS OR SIMULATION
     * Buck efficiency has to be calculated
     * Only active, if SIMULATION-define is set && WORKAROUND-define is not set!
     */
    P_out = V_out * I_out;
    if (P_in > 0)
    {
        efficiency = (P_out * 100 / P_in);
    }
    else
    {
        efficiency = 0;
    }
    // #endif

    /* Calculate sensor values for MPP */
    dV = V_in - V_in_lastCycle;
    dI = I_in - I_in_lastCycle;

    if (dV == 0)
    {
        dI_dV_ratio = 0;
    }
    else
    {
        dI_dV_ratio = dI / dV;
    }

    if (V_in == 0)
    {
        I_V_ratio = 0;
    }
    else
    {
        I_V_ratio = I_in / V_in;
    }

    /* Set all sensor values from acutall cylce for next cycle */
    V_in_lastCycle = V_in;
    V_out_lastCycle = V_out;
    I_in_lastCycle = I_in;
    I_out_lastCycle = I_out;
}

/** Potentiometer
 * get state in percent
 * @param pin Pin where Potentiometer is connected
 * @return Poti state in percent (0 % --> 100 %)
 */
int edugrid_measurement::getPotentiometerPercent(uint8_t pin)
{ // get Potentiometer value in percent (0 % --> 100 %)
    volatage_buffer = internal_getVoltage(pin);

    return (volatage_buffer * 100 / ADC_MAX_VOLTAGE);
}

/** Encoder
 * Init the KY-040 encoder
 */
void edugrid_measurement::encoderInit()
{
    attachInterrupt(digitalPinToInterrupt(PWM_ENCODER_PIN_SW), encoder_pressed_ISR, RISING);

    ESP32Encoder::useInternalWeakPullResistors = UP;
    encoder.attachHalfQuad(PWM_ENCODER_PIN_DT, PWM_ENCODER_PIN_CLK);
    encoder.setCount(100);
}

/** Check if encoder is CW or CCW rotated
 */
void edugrid_measurement::getEncoderChange()
{
    if (millis() > ENCODER_DEBOUNCE_DELAY_MS) // activate encoder time-delayed
    {
        encoder_new_state = encoder.getCount();

        if (encoder_new_state > encoder_old_state)
        {
            // CW
            edugrid_pwm_control::pwmIncrementDecrement(5);
            encoder_old_state = encoder_new_state;
        }
        else if (encoder_new_state < encoder_old_state)
        {
            // CCW
            edugrid_pwm_control::pwmIncrementDecrement(-5);
            encoder_old_state = encoder_new_state;
        }
    }
}

/** Interrupt for Encoder is pressed
 *
 */
void edugrid_measurement::encoder_pressed_ISR()
{
    if ((millis() - encoder_time_SW) >= ENCODER_DEBOUNCE_DELAY_SW_MS) // && !digitalRead(PWM_ENCODER_PIN_CLK) && !digitalRead(PWM_ENCODER_PIN_DT))
    {
        encoder_time_SW = millis();
        // edugrid_mpp_algorithm::toggle_mode_state();
    }
}