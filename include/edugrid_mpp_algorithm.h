/*************************************************************************
 * @file edugrid_mpp_algorithm.h
 * @date 2023/11/05
 * @reference http://ww1.microchip.com/downloads/en/AppNotes/00001521A.pdf
 *
 ************************************************************************/

#ifndef EDUGRID_MPP_ALGORITHM_H_
#define EDUGRID_MPP_ALGORITHM_H_

/*************************************************************************
 * Include
 ************************************************************************/
#include <Arduino.h>
#include <edugrid_states.h>
#include <edugrid_measurement.h>
#include <edugrid_pwm_control.h>
#include <edugrid_lcd.h>

/*************************************************************************
 * Define
 ************************************************************************/
#define RATIO_DEVIATION (0.95) // define that a deviation in +/- of the dI/dV-ratio is netherless a mpp!
#define PWM_INCREASE (1)       // increase voltage with this factor to find mpp
#define PWM_DECREASE (-1)      // decrease voltage with this factor to find mpp

#define MODE_AUTO (0)
#define MODE_MANUALLY (1)

#define MPP_CYCLE_TIME_DEFAULT (500) // [ms] TODO set later to a lower value

/*************************************************************************
 * Class
 ************************************************************************/

/**
 * Class with static members for all mpp algorithm tasks
 */
class edugrid_mpp_algorithm
{
public:
    /* MPPT algorithm*/
    static int find_mpp(void);
    /* Getter / Setter */
    static uint8_t get_mode_state(void);
    static void set_mode_state(uint8_t mode);
    static void toggle_mode_state();
    /* Internal telemetry*/
    static void serial_debug(void);
    /* Variable*/
    static uint32_t mpp_cycle_time;

private:
    static int8_t mpp_retVal;
    /* State */
    static uint8_t mode_state;

protected:
};

#endif /* EDUGRID_MPP_ALGORITHM_H_ */