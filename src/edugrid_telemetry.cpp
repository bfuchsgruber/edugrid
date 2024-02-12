/*************************************************************************
 * @file edugrid_telemetry.cpp
 * @date 2023/11/26
 *
 ************************************************************************/

/*************************************************************************
 * Include
 ************************************************************************/
#include <edugrid_telemetry.h>

/*************************************************************************
 * Define
 ************************************************************************/

/*************************************************************************
 * Variable Definition
 ************************************************************************/

/*************************************************************************
 * Function Definition
 ************************************************************************/

/** Print telemetry data to serial
 */
void edugrid_telemtry::telemetryPrint(void)
{
    /* Telemetry for PWM control */
    Serial.println("* ------------------------------------ *");
    Serial.println("* PWM CONTROL *");
    Serial.println("* ------------------------------------ *");
    Serial.print("Freq / Hz: ");
    Serial.println(edugrid_pwm_control::getFrequency());
    Serial.print("PWM / %: ");
    Serial.println(edugrid_pwm_control::getPWM());

    /* Telemetry for sensor readings */
    Serial.println("* ------------------------------------ *");
    Serial.println("* SENSORS *");
    Serial.println("* ------------------------------------ *");
    Serial.print("V_in = ");
    Serial.println(edugrid_measurement::V_in);
    Serial.print("V_out = ");
    Serial.println(edugrid_measurement::V_out);
    Serial.print("I_in = ");
    Serial.println(edugrid_measurement::I_in);
    Serial.print("I_out = ");
    Serial.println(edugrid_measurement::I_out);
    Serial.print("Efficiency = ");
    Serial.println(edugrid_measurement::efficiency);

    /* Telemetry for MPP algorithm */
    Serial.println("* ------------------------------------ *");
    Serial.println("* MPP ALGORITHM *");
    Serial.println("* ------------------------------------ *");
    Serial.print("MPP Mode: ");
    if (edugrid_mpp_algorithm::get_mode_state() == MODE_AUTO)
    {
        Serial.println("Automatic");
    }
    else
    {
        Serial.println("Manual");
    }
    Serial.print("V_in_old = ");
    Serial.println(edugrid_measurement::V_in_lastCycle);
    Serial.print("I_in_old = ");
    Serial.println(edugrid_measurement::I_in_lastCycle);
    Serial.print("dV = ");
    Serial.println(edugrid_measurement::dV);
    Serial.print("dI = ");
    Serial.println(edugrid_measurement::dI);
    Serial.print("dI_dV_ratio = ");
    Serial.println(edugrid_measurement::dI_dV_ratio);
    Serial.print("I_V_ratio = ");
    Serial.println(edugrid_measurement::I_V_ratio);
    edugrid_mpp_algorithm::serial_debug();

    /* Telemetry for Webserver */
    Serial.println("* ------------------------------------ *");
    Serial.println("* WEBSERVER & WEBSOCKET *");
    Serial.println("* ------------------------------------ *");
    Serial.print("WLAN Name: ");
    Serial.println(edugrid_filesystem::config_wlan_ssid.c_str());
    Serial.print("WLAN Password: ");
    Serial.println(edugrid_filesystem::config_wlan_pw.c_str());
    Serial.print("JSON Publisher: ");
    String json_doc;
    serializeJson(edugrid_webserver::doc, json_doc);
    Serial.println(json_doc);

    /* Telemetry for misc values */
    Serial.println("* ------------------------------------ *");
    Serial.println("* MISC *");
    Serial.println("* ------------------------------------ *");
    Serial.print("Simulation ");
    Serial.println(edugrid_simulation::getSimulationState_str());
    Serial.print("LCD state = ");
    Serial.println(edugrid_lcd::getLcdInitState());

    /* END OF TELEMETRY */
    Serial.println("\n"); // two-time newline
}