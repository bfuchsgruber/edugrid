/*************************************************************************
 * @file edugrid_telemtry.h
 * @date 2023/11/26
 *
 ************************************************************************/

#ifndef EDUGRID_TELEMETRY_H_
#define EDUGRID_TELEMETRY_H_

/*************************************************************************
 * Include
 ************************************************************************/
#include <Arduino.h>
#include <edugrid_states.h>
#include <edugrid_pwm_control.h>
#include <edugrid_mpp_algorithm.h>
#include <edugrid_webserver.h>

/*************************************************************************
 * Define
 ************************************************************************/

/*************************************************************************
 * Class
 ************************************************************************/
class edugrid_telemtry
{
public:
    static void telemetryPrint(void);

private:
protected:
};

#endif /* EDUGRID_TELEMETRY_H_ */