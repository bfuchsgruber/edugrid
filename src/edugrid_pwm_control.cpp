/*************************************************************************
 * @file edugrid_pwm_control.cpp
 * @date 2023/11/05
 *
 ************************************************************************/

/*************************************************************************
 * Include
 ************************************************************************/
#include <edugrid_pwm_control.h>
#include <Arduino.h>

/*************************************************************************
 * Define
 ************************************************************************/

/*************************************************************************
 * Variable Definition
 ************************************************************************/
uint8_t edugrid_pwm_control::pwm_power_converter = 0;
int edugrid_pwm_control::frequency_power_converter = 0;
int edugrid_pwm_control::power_converter_pin = 0;

/*************************************************************************
 * Function Definition
 ************************************************************************/

/** Set the duty cycle of the converter PWM signal
 * PWM duty cycle can only be set in manuall modus!
 * @param pwm_in Duty Cyle in %
 * @param auto_mode True if request is from MPPT algorithm (Default: false)
 */
void edugrid_pwm_control::setPWM(uint8_t pwm_in, bool auto_mode)
{
    if ((edugrid_mpp_algorithm::get_mode_state() == MODE_MANUALLY) || auto_mode)
    {
        // range for PWM value: 0 % --> 100 %
        if (pwm_in >= PWM_ABS_MAX)
        {
            edugrid_pwm_control::pwm_power_converter = PWM_ABS_MAX;
        }
        else if (pwm_in <= PWM_ABS_MIN)
        {
            edugrid_pwm_control::pwm_power_converter = PWM_ABS_MIN;
        }
        else
        {
            edugrid_pwm_control::pwm_power_converter = pwm_in;
        }

        // use getter method and set PWM to internal PWM function
        ledcWrite(TIMER_PWM_POWER_CONVERTER, PWM_RESOLUTION * getPWM_normalized());
        Serial.print("| OK | ");
        Serial.print(getPWM());
        Serial.println(" % PWM");
    }
    else
    {
        Serial.println("|FAIL| PWM can only set in manuall mode");
    }
}

/** Set frequency of the PWM
 * @param freq Frequency in Hz
 */
void edugrid_pwm_control::setFrequency(float freq)
{
    // set a frequency in Hz
    frequency_power_converter = freq;
}

/** Set GPIO pi where the PWM is generated
 * @param pin GPIO pin of the esp32
 */
void edugrid_pwm_control::setPin(int pin)
{
    power_converter_pin = pin;
}

/** Get duty cycle of PWM in range 0 % --> 100 %
 * @return Duty Cycle in %
 */
uint8_t edugrid_pwm_control::getPWM()
{
    return pwm_power_converter;
}

/** Get duty cycle of PWM in range 0 --> 1
 * @return Duty cycle from 0 --> 1 without unit
 */
float edugrid_pwm_control::getPWM_normalized()
{
    // Serial.println((float) get_pwm() / 100.0);
    return (float)getPWM() / 100.0;
}

/** Get frequency of PWM in Hz
 * @return Frequency in Hz
 */
float edugrid_pwm_control::getFrequency()
{
    return frequency_power_converter;
}

/** Get frequency of PWM in kHz
 * @return Frequency in kHz
 */
uint8_t edugrid_pwm_control::getFrequency_kHz()
{
    return frequency_power_converter / 1000.0;
}

/** Init the PWM signal
 * @param freq Frequency of the PWM signal
 * @param pin GPIO pin where the pulse is generated
 */
void edugrid_pwm_control::initPwmPowerConverter(int freq, int pin)
{
    frequency_power_converter = freq;
    power_converter_pin = pin;

    ledcSetup(TIMER_PWM_POWER_CONVERTER, frequency_power_converter, 8);
    ledcAttachPin(power_converter_pin, TIMER_PWM_POWER_CONVERTER);
    setPWM(PWM_ABS_MIN);

    /* Activate gate driver with setting GPIO 32 to HIGH (!SD = !Shutdown PIN --> LOW sensitive!)*/
    pinMode(PIN_SD_GATE_DRIVER, OUTPUT);
    digitalWrite(PIN_SD_GATE_DRIVER, HIGH);
    Serial.println("| OK | Gate driver activated");
}

/** Set a PWM increment
 * @param step Set the increment/decrement step, [step] = %
 * @param auto_mode True if request is from MPPT algorithm (Default: false)
 */
void edugrid_pwm_control::pwmIncrementDecrement(int step, bool auto_mode)
{
    setPWM(getPWM() + step, auto_mode);
}