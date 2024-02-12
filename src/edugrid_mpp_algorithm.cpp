/*************************************************************************
 * @file edugrid_timers.cpp
 * @date 2023/11/05
 *
 ************************************************************************/

/*************************************************************************
 * Include
 ************************************************************************/
#include <Arduino.h>
#include <edugrid_mpp_algorithm.h>

/*************************************************************************
 * Define
 ************************************************************************/

/*************************************************************************
 * Variable Definition
 ************************************************************************/
uint32_t edugrid_mpp_algorithm::mpp_cycle_time = MPP_CYCLE_TIME_DEFAULT;
int8_t edugrid_mpp_algorithm::mpp_retVal = -1;
uint8_t edugrid_mpp_algorithm::mode_state = MODE_MANUALLY;

/*************************************************************************
 * Function Definition
 ************************************************************************/

/** Find mpp according to selected algorithm
 *
 */
int edugrid_mpp_algorithm::find_mpp(void)
{
    /* decide position related to mpp */
    if (edugrid_measurement::dI_dV_ratio == -edugrid_measurement::I_V_ratio)
    {
        /* at MPP --> return 0*/
        mpp_retVal = 0;
    }
    else if (edugrid_measurement::dI_dV_ratio > -edugrid_measurement::I_V_ratio)
    {
        /* left of MPP --> return 1
         * Increase PWM DC
         */
        edugrid_pwm_control::pwmIncrementDecrement(PWM_INCREASE, true);
        mpp_retVal = 1;
    }
    else if (edugrid_measurement::dI_dV_ratio < -edugrid_measurement::I_V_ratio)
    {
        /* right of MPP --> return 2
         * Decrease PWM DC
         */
        edugrid_pwm_control::pwmIncrementDecrement(PWM_DECREASE, true);
        mpp_retVal = 2;
    }

    // serial_debug();
    return mpp_retVal;
}

/** Get MPPT mode
 * @return MODE_AUTO / MODE_MANUALLY (see defines from edugrid_mpp_algorithm)
 */
uint8_t edugrid_mpp_algorithm::get_mode_state(void)
{
    return edugrid_mpp_algorithm::mode_state;
}

/** Set Mode (manually / automatic)
 * @param mode MODE_AUTO / MODE_MANUALLY (see defines from edugrid_mpp_algorithm)
 */
void edugrid_mpp_algorithm::set_mode_state(uint8_t mode)
{
    edugrid_mpp_algorithm::mode_state = mode;
    Serial.print("| OK | MPP-Mode set to: ");

    /* get MPP mode and display it */
    if (get_mode_state() == MODE_AUTO)
    {
        Serial.println("AUTOMATIC");
    }
    else
    {
        Serial.println("MANUAL");
    }
}

/** Switch modes
 * @param None
 */
void edugrid_mpp_algorithm::toggle_mode_state()
{
    switch (get_mode_state())
    {
    case MODE_AUTO:
        set_mode_state(MODE_MANUALLY);
        break;
    case MODE_MANUALLY:
        set_mode_state(MODE_AUTO);
        break;
    }
}

/** Print all MPPT-related values
 *
 */
void edugrid_mpp_algorithm::serial_debug(void)
{
    Serial.print("mpp_retVal = ");
    Serial.println(mpp_retVal);
}