/*************************************************************************
 * @file edugrid_webserver.h
 * @date 2023/11/13
 *
 ************************************************************************/

#ifndef EDUGRID_WEBSERVER_H_
#define EDUGRID_WEBSERVER_H_

/*************************************************************************
 * Include
 ************************************************************************/
#include <Arduino.h>
#include <WiFi.h>
// #include <ESPAsyncTCP.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <AsyncElegantOTA.h>
#include <ArduinoJson.h>
#include <edugrid_pwm_control.h>
#include <edugrid_filesystem.h>
#include <edugrid_mpp_algorithm.h>
#include <edugrid_states.h>
#include <edugrid_mpp_algorithm.h>
#include <edugrid_logging.h>

/*************************************************************************
 * Define
 ************************************************************************/
// #define SSID "edugrid_EduGrid"
// #define PASSWORD "edugrid_PW1!"
#define DNS_DOMAIN "edugrid"

/* HTML IDs */
#define WEBSERVER_ID_MPP_SWITCH ("1")
#define WEBSERVER_ID_PWM_INCREMENT ("2")
#define WEBSERVER_ID_PWM_DECREMENT ("3")
#define WEBSERVER_ID_PWM_SLIDER ("4")
#define WEBSERVER_ID_SIMULATION_LABEL ("simulation_label")
#define WEBSERVER_ID_MODE_LABEL ("mode_label")
#define WEBSERVER_ID_LOGGING_LABEL ("logging_label")
#define WEBSERVER_ID_PWM_FREQ_LABEL ("freq_label")
#define WEBSERVER_ID_SAFE_BUTTON ("safe_button")

/* Filesystem paths */
#define WEBSERVER_HOME_PATH ("/www/index.html")
#define WEBSERVER_STYLE_PATH ("/www/style.css")
#define WEBSERVER_JS_PATH ("/www/script.js")
#define WEBSERVER_FILE_PATH ("/www/file.html")
#define WEBSERVER_ADMIN_PATH ("/www/admin.html")

/*************************************************************************
 * Class
 ************************************************************************/
class edugrid_webserver
{
public:
    static void initWiFi(void);
    static void webSocketLoop(void);
    static String humanReadableSize(const size_t bytes);
    static StaticJsonDocument<512> doc;

private:
    static String processor(const String &var);
    static void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    static String listFiles(bool ishtml);
    static void setWiFiCredentials(String ssid, String pw);
    static String _id;
    static String _state;
    static String _state2;
    static String JSON_str;
    static String wifi_name;
    static String wifi_password;

protected:
};

#endif /* EDUGRID_WEBSERVER_H_ */