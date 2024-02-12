/*************************************************************************
 * @file edugrid_pwm_control.h
 * @date 2023/11/05
 *
 ************************************************************************/

#ifndef EDUGRID_PWM_CONTROL_H_
#define EDUGRID_PWM_CONTROL_H_

/*************************************************************************
 * Include
 ************************************************************************/
#include <Arduino.h>
#include <edugrid_states.h>
#include <edugrid_mpp_algorithm.h>

/*************************************************************************
 * Define
 ************************************************************************/
#define TIMER_PWM_POWER_CONVERTER (0)
#define PWM_RESOLUTION (255)
#define PWM_ABS_MIN (5)  // [%]
#define PWM_ABS_MAX (95) // [%]
#define PIN_SD_GATE_DRIVER (32)

/*************************************************************************
 * Class
 ************************************************************************/

/**
 * Class with static members for all PWM signal tasks
 */
class edugrid_pwm_control
{
public:
    /* Getter / Setter */
    static void setPWM(uint8_t pwm_in, bool auto_mode = false); // 0 % --> 100 %
    static void setFrequency(float freq);                       // [Hz]
    static void setPin(int pin);
    static uint8_t getPWM();           // 0 % --> 100 %
    static float getPWM_normalized();  // 0.0 --> 1.0
    static float getFrequency();       // [Hz]
    static uint8_t getFrequency_kHz(); // [kHz]
    static void pwmIncrementDecrement(int step = 5, bool auto_mode = false);
    /* Init */
    static void initPwmPowerConverter(int freq, int pin);

private:
    static uint8_t pwm_power_converter;
    static int frequency_power_converter;
    static int power_converter_pin;

protected:
};

#endif /* EDUGRID_PWM_CONTROL_H_ */