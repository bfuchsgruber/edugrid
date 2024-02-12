/************************************************************************
 * @file main.cpp
 * @date 2023/11/05
 *
 ***********************************************************************/

/************************************************************************
 * Include
 ************************************************************************/
#include <Arduino.h>
#include <edugrid_pwm_control.h>
#include <edugrid_mpp_algorithm.h>
#include <edugrid_measurement.h>
#include <edugrid_webserver.h>
#include <edugrid_filesystem.h>
#include <edugrid_states.h>
#include <edugrid_lcd.h>
#include <edugrid_logging.h>

#include <edugrid_telemetry.h>

/************************************************************************
 * Define
 ************************************************************************/
#define BAUD_RATE (115200)
#define CYCLE_TIME_TASK_0_MS (1000) // [ms]
#define CYCLE_TIME_TASK_1_MS (10)   // [ms]

#define CONVERTER_FREQUENCY (40000)  // [Hz]
#define PIN_POWER_CONVERTER_PWM (33) // GPIO PIN NUMBER
#define PWM_DUTYCYCLE_INIT (50)      // [%]

#define BLUE_LED_STATE_PIN (13) // GPIO No

/************************************************************************
 * Variable Definition
 ************************************************************************/
TaskHandle_t core2; // WebSocket & WiFi
TaskHandle_t core3; // MPPT Algorithms and Sensors

/************************************************************************
 * Function Definition
 ***********************************************************************/

/** Seperate task (2) for updating values to the webSocket server */
void coreTwo(void *pvParameters)
{
  while (1)
  {
    edugrid_webserver::webSocketLoop();
    delay(CYCLE_TIME_TASK_1_MS);
  }
}

/** Seperate task (3) for MPP algorithm */
void coreThree(void *pvParameters)
{
  while (1)
  {
    /* Get all sensor readings (PV and Load Voltage/Current) */
    edugrid_measurement::getSensors();

    /* Check Watchdog for Battery Load */
    edugrid_measurement::batteryWatchdog();

    /* Get potential PWM change from encoder */
    edugrid_measurement::getEncoderChange();

    /* MPP Modes */
    switch (edugrid_mpp_algorithm::get_mode_state())
    {
    case MODE_AUTO:
      edugrid_mpp_algorithm::find_mpp();
      break;

    case MODE_MANUALLY:
      break;
    }

#ifdef EDUGRID_TELEMETRY_ON
    edugrid_telemtry::telemetryPrint();
#endif

    delay(edugrid_mpp_algorithm::mpp_cycle_time);
  }
}

void setup()
{
  /* Serial setup */
  Serial.begin(BAUD_RATE);

  /* Filesystem setup */
  edugrid_filesystem::init_filesystem();
  edugrid_filesystem::loadConfig();
  edugrid_simulation::readSimulationStateFromFlash();

  /* Warn at active simulation */
  if (edugrid_simulation::getSimulationState())
  {
    Serial.println("|WARN| Simulation active");
  }

  /* Webserver setup */
  edugrid_webserver::initWiFi();

  /* PWM setup */
  edugrid_pwm_control::initPwmPowerConverter(CONVERTER_FREQUENCY, PIN_POWER_CONVERTER_PWM);

  /* LCD setup */
  edugrid_lcd::lcdInit();

  /* ADS1115 extern adc setup */
  edugrid_measurement::external_adcInit();

  /* Encoder setup */
  edugrid_measurement::encoderInit();

  /* end setup */
  edugrid_pwm_control::setPWM(PWM_DUTYCYCLE_INIT);
  // edugrid_lcd::lcdClear();

  /* Start additional cores */
  xTaskCreatePinnedToCore(coreTwo, "coreTwo", 10000, NULL, 0, &core2, 0);
  xTaskCreatePinnedToCore(coreThree, "coreThree", 10000, NULL, 0, &core3, 0);

  delay(1500);
  Serial.print("| OK | EduGrid (");
  Serial.print(EDUGRID_VERSION);
  Serial.println(") running");
#ifdef OTA_UPDATES_ENABLE
  Serial.println("| OK | OTA Updates are enabled\n");
#endif
  /* Blue LED on IO 13 --> GPIO 33*/
  pinMode(BLUE_LED_STATE_PIN, OUTPUT);
  digitalWrite(BLUE_LED_STATE_PIN, HIGH);
}

/** Seperate task (1) for LCD and misc things */
void loop()
{
  /* Update LCD Panel */
  edugrid_lcd::lcdWriteText(5, 0, (String)edugrid_pwm_control::getFrequency_kHz() + " kHz   " + (String)edugrid_pwm_control::getPWM() + " %");
  edugrid_lcd::lcdWriteText(5, 1, (String)edugrid_measurement::P_in + " W");
  edugrid_lcd::lcdWriteText(5, 2, (String)edugrid_measurement::P_out + " W (" + (String)edugrid_measurement::efficiency + "%)");

  /* get MPP mode and display it */
  if (edugrid_mpp_algorithm::get_mode_state() == MODE_AUTO)
  {
    edugrid_lcd::lcdWriteText(5, 3, "AUTOMATIC");
  }
  else
  {
    edugrid_lcd::lcdWriteText(5, 3, "            ");
    edugrid_lcd::lcdWriteText(5, 3, "MANUAL");
  }

  /* Logging into flash */
  edugrid_logging::appendLog((String)edugrid_measurement::V_in,
                             (String)edugrid_measurement::V_out,
                             (String)edugrid_measurement::I_in,
                             (String)edugrid_measurement::I_out);

  /* Cycle delay */
  delay(CYCLE_TIME_TASK_0_MS);
}
