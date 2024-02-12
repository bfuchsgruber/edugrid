/*************************************************************************
 * @file edugrid_measurements.h
 * @date 2023/11/05
 *
 ************************************************************************/

#ifndef EDUGRID_MEASUREMENTS_H_
#define EDUGRID_MEASUREMENTS_H_

/*************************************************************************
 * Include
 ************************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <ADS1X15.h>
#include <ESP32Encoder.h>
#include <edugrid_states.h>
#include <edugrid_pwm_control.h>
#include <edugrid_mpp_algorithm.h>
#include <edugrid_simulation.h>

/*************************************************************************
 * Define
 ************************************************************************/
/* Battery Cut-Off voltage */
#define BATTERY_CUT_OFF_VOLTAGE (14.80) // [V]

/* esp32 intern ADC */
#define ADC_MAX_RESOLUTION (4095)
#define ADC_MAX_VOLTAGE (3.3)

// ACS712 hal-based current sensor
#define ACS712_OFFSET (2.5)
// I_max = 5 A
#define ACS712_5A_SENSIVITY (0.185)
// I_max = 20 A
#define ACS712_20A_SENSIVITY (0.100)
// I_max = 30 A
#define ACS712_30A_SENSIVITY (0.066)
// used range
#define ACS712_RANGE_SET (30) // [A]
// cal factors
#define ACS712_CAL_OFFSET (0)         // 0.35
#define ACS712_CAL_FACTOR (2.5 / 2.1) // 2.5 / 2.3

/* ADS1X15 external ADC */
#define ADS_ADDRESS (0x48)
#define ADS_CHANNEL (4)

/* Encoder */
/* GND = GND
 * +   = +3V
 * CLK = Output A
 * DT  = Output B
 * SW  = Switch PIN (Button Pin)
 */
#define PWM_ENCODER_PIN_CLK (25)
#define PWM_ENCODER_PIN_DT (26)
#define PWM_ENCODER_PIN_SW (27)
#define ENCODER_DEBOUNCE_DELAY_MS (3000)
#define ENCODER_DEBOUNCE_DELAY_SW_MS (500)

/* Pins for measurements */
#define PIN_VOLTAGE_IN (3)  // external ADC pins
#define PIN_VOLTAGE_OUT (1) // external ADC pins
#define PIN_CURRENT_IN (2)  // external ADC pins
#define PIN_CURRENT_OUT (0) // external ADC pins

/* Voltage divider ratio */
#define DIVIDER_VOLTAGE_IN (2051.0 / 51.0)
#define DIVIDER_VOLTAGE_OUT (24.5)
#define DIVIDER_CURRENT_IN (1.33)
#define DIVIDER_CURRENT_OUT (1.33)

/*************************************************************************
 * Class
 ************************************************************************/

/** edugrid_measurement
 * Class with static members for all measurement tasks
 */
class edugrid_measurement
{
public:
    /* Battery Watchdog */
    static void batteryWatchdog();
    static bool watchdog_off_request;
    /* Internal ADCs */
    static float internal_getVoltage(uint8_t pin);
    static int getPotentiometerPercent(uint8_t pin);
    /* ADS1015 */
    static void external_adcInit();
    static float external_getVoltage(uint8_t pin, float ratio = 1);
    static float external_getCurrent(uint8_t pin, uint8_t range = 5, float ratio = 1);
    /* Encoder */
    static void encoderInit();
    static void getEncoderChange();
    /* Voltage / Current measurements */
    static float getVoltagePV();
    static float getVoltageLoad();
    static float getCurrentPV();
    static float getCurrentLoad();
    static void getSensors();
    /* Measurements */
    static float V_in;
    static float V_out;
    static float I_in;
    static float I_out;
    /* Measurements last cycle --> for mpp algorithm*/
    static float V_in_lastCycle;
    static float V_out_lastCycle;
    static float I_in_lastCycle;
    static float I_out_lastCycle;
    /* Calculations */
    static float P_in;
    static float P_out;
    static uint8_t efficiency; // [%]
    static float dV;
    static float dI;
    static float dI_dV_ratio;
    static float I_V_ratio;

private:
    static float volatage_buffer;
    static int16_t ads_raw;
    static float ads_voltage;
    static long encoder_old_state;
    static long encoder_new_state;
    volatile static long encoder_time_SW;
    static void encoder_pressed_ISR();

protected:
};

#endif /* edugrid_MEASUREMENTS_H_ */