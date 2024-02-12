/*************************************************************************
 * @file edugrid_simulation.h
 * @date 2023/12/13
 *
 ************************************************************************/

#ifndef EDUGRID_SIMULATION_H_
#define EDUGRID_SIMULATION_H_

/*************************************************************************
 * Include
 ************************************************************************/
#include <Arduino.h>
#include <edugrid_states.h>
#include <edugrid_pwm_control.h>
#include <edugrid_filesystem.h>

/*************************************************************************
 * Define
 ************************************************************************/

#define SIMU_VALUES_MAX (3239)

#define ln(x) log(x)

#define n_PV (1)
#define kT_q (26e-3) // k*t/q = approx. 26 mV
#define I_L (0.5)
#define I_0 (1e-10)

#define ETA_BUCK (95)

/*************************************************************************
 * Class
 ************************************************************************/
class edugrid_simulation
{
public:
    static float getVoltagePV_simulation();
    static float getVoltageLoad_simulation();
    static float getCurrentPV_simulation();
    static float getCurrentLoad_simulation();
    static String getSimulationState_str();
    static void readSimulationStateFromFlash();
    static bool getSimulationState();
    static void activateSimulation();
    static void deactivateSimulation();
    static void toggleSimulation();
    

private:
    static float u_in_sim[SIMU_VALUES_MAX];
    static float i_in_sim[SIMU_VALUES_MAX];
    /* Array counters */
    static int cycle_counter;
    static int array_counter;
    static bool simulation_state;

protected:
};

#endif /* EDUGRID_SIMULATION_H_ */