/*************************************************************************
 * @file edugrid_webserver.cpp
 * @date 2023/11/13
 *
 *************************************************************************/

/*************************************************************************
 * Include
 ************************************************************************/
#include <edugrid_webserver.h>

/*************************************************************************
 * Define
 ************************************************************************/

/*************************************************************************
 * Variable Definition
 ************************************************************************/
String edugrid_webserver::_id = "";
String edugrid_webserver::_state = "";
String edugrid_webserver::_state2 = "";
String edugrid_webserver::JSON_str = "";
String edugrid_webserver::wifi_name = "";
String edugrid_webserver::wifi_password = "";
StaticJsonDocument<512> edugrid_webserver::doc;

WebSocketsServer webSocket = WebSocketsServer(81);
AsyncWebServer server(80);

const char *PARAM_INPUT_1 = "ID";
const char *PARAM_INPUT_2 = "STATE";
const char *PARAM_INPUT_3 = "STATE2";

/*************************************************************************
 * Function Definition
 ************************************************************************/

/** Handle all dynamic websocket changes
 * @param &var Reference to variable, which is used in HTML file
 * @return String to paste at position of var in HTML file
 */
String edugrid_webserver::processor(const String &var)
{
    String buttons = "";
    if (var == "BUTTONPLACEHOLDER")
    {
        return (String)EDUGRID_VERSION;
    }
    if (var == "FILELIST")
    {
        return listFiles(true);
    }
    if (var == "FREESPIFFS")
    {
        return humanReadableSize((LittleFS.totalBytes() - LittleFS.usedBytes()));
    }

    if (var == "USEDSPIFFS")
    {
        return humanReadableSize(LittleFS.usedBytes());
    }

    if (var == "TOTALSPIFFS")
    {
        return humanReadableSize(LittleFS.totalBytes());
    }
    return String();
}

/** Start the esp32 asynchron webserver and set communication interfaces to according HTML-IDs
 *
 */
void edugrid_webserver::initWiFi(void)
{
    WiFi.softAP(edugrid_filesystem::config_wlan_ssid.c_str(), edugrid_filesystem::config_wlan_pw.c_str());
    /* Config WiFi AP */
    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    // Websocket
    webSocket.begin();

    // Load file from LittleFS and start server
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, WEBSERVER_HOME_PATH, String(), false, processor); });

    // Load CSS from LittleFS
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, WEBSERVER_STYLE_PATH, String(), false, processor); });

    // Load JS from LittleFS
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, WEBSERVER_JS_PATH, String(), false, processor); });

    // Load file from LittleFS and start admin page
    server.on("/admin", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, WEBSERVER_ADMIN_PATH, String(), false, processor); });

    // Load file from LittleFS and start file upload/download
    server.on("/file", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, WEBSERVER_FILE_PATH, String(), false, processor); });

    // Load file from LittleFS and start file handler page
    server.on("/filehandle", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
   

      if (request->hasParam("name") && request->hasParam("action")) {
        const char *fileName = request->getParam("name")->value().c_str();
        const char *fileAction = request->getParam("action")->value().c_str();

        logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url() + "?name=" + String(fileName) + "&action=" + String(fileAction);

        if (!LittleFS.exists(fileName)) {
          Serial.println(logmessage + " ERROR: file does not exist");
          request->send(400, "text/plain", "ERROR: file does not exist");
        } else {
          Serial.println(logmessage + " file exists");
          if (strcmp(fileAction, "download") == 0) {
            logmessage += " downloaded";
            request->send(LittleFS, fileName, "application/octet-stream");
          } else if (strcmp(fileAction, "delete") == 0) {
            logmessage += " deleted";
            LittleFS.remove(fileName);
            request->send(200, "text/plain", "Deleted File: " + String(fileName));
          } else {
            logmessage += " ERROR: invalid action param supplied";
            request->send(400, "text/plain", "ERROR: invalid action param supplied");
          }
          Serial.println(logmessage);
        }
      } else {
        request->send(400, "text/plain", "ERROR: name and action params required");
      } });

    server.on("/listfiles", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
                request->send(200, "text/plain", listFiles(true)); });

    // run handleUpload function when any file is uploaded
    server.on(
        "/upload", HTTP_POST, [](AsyncWebServerRequest *request)
        { request->send(200); },
        handleUpload);

    // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    server.on("/updatevalues", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
                  if (request->hasParam(PARAM_INPUT_1))
                  {
                      _id = request->getParam(PARAM_INPUT_1)->value();
                      _state = request->getParam(PARAM_INPUT_2)->value();
                      if (_id.equals(WEBSERVER_ID_MPP_SWITCH)) // MPP Switch
                      {
                          edugrid_mpp_algorithm::toggle_mode_state();
                      }
                      else if (_id.equals(WEBSERVER_ID_PWM_INCREMENT))
                      {
                          edugrid_pwm_control::pwmIncrementDecrement(5);
                      }
                      else if (_id.equals(WEBSERVER_ID_PWM_DECREMENT))
                      {
                          edugrid_pwm_control::pwmIncrementDecrement(-5);
                      }
                      else if (_id.equals(WEBSERVER_ID_PWM_SLIDER))
                      {
                          edugrid_pwm_control::setPWM(_state.toInt());
                      }
                      else if (_id.equals(WEBSERVER_ID_SIMULATION_LABEL))
                      {
                          edugrid_simulation::toggleSimulation();
                          Serial.print("|WARN| Simulation ");
                          Serial.println(edugrid_simulation::getSimulationState_str());
                      }
                      else if (_id.equals(WEBSERVER_ID_MODE_LABEL))
                      {
                          edugrid_mpp_algorithm::toggle_mode_state();
                      }
                      else if (_id.equals(WEBSERVER_ID_LOGGING_LABEL))
                      {
                          edugrid_logging::toggleLogging();
                      }
                      else if (_id.equals(WEBSERVER_ID_SAFE_BUTTON)) // 192.168.1.1/admin --> WiFi Credentials
                      {
                          /* _state == SSID
                           * _state2 == PW
                           */
                          _state2 = request->getParam(PARAM_INPUT_3)->value();
                          Serial.println("| OK | WLAN SSID & PW changed");
                          Serial.print("       SSID: ");
                          Serial.println(_state);
                          Serial.print("       Password: ");
                          Serial.println(_state2);
                          edugrid_filesystem::setWiFiCredentials(_state, _state2);
                          Serial.println("|WARN| System restarting...");
                          delay(3000);
                          ESP.restart();
                      }
                      else if (_id.equals(WEBSERVER_ID_PWM_FREQ_LABEL)){
                        Serial.println("LABEL");
                      }
                  }

                  request->send(200, "text/plain", "OK");
                  // end server
              });

#ifdef OTA_UPDATES_ENABLE
    // Start ElegantOTA
    AsyncElegantOTA.begin(&server);
#endif

    // Start server
    server.begin();

    Serial.print("| OK | edugrid EduGrid Webserver started at ");
    Serial.println(WiFi.softAPIP());
    Serial.print("       Name: ");
    Serial.print(edugrid_filesystem::config_wlan_ssid.c_str());
    Serial.print(" - Password: ");
    Serial.println(edugrid_filesystem::config_wlan_pw.c_str());
}

/** Broadcast a JSON doc with values periodically to the websocket
 */
void edugrid_webserver::webSocketLoop(void)
{

    // AsyncElegantOTA.loop();
    webSocket.loop();
    doc["pwm"] = (String)edugrid_pwm_control::getPWM() + " %";
    doc["pwm_raw"] = edugrid_pwm_control::getPWM();
    doc["freq"] = (String)edugrid_pwm_control::getFrequency_kHz() + " kHz";
    doc["mode"] = (String)edugrid_mpp_algorithm::get_mode_state();

    doc["logging"] = edugrid_logging::getLogState_str();

    doc["simulation"] = edugrid_simulation::getSimulationState_str();

    doc["current_in"] = (String)edugrid_measurement::I_in + " A";
    doc["current_out"] = (String)edugrid_measurement::I_out + " A";
    doc["voltage_in"] = (String)edugrid_measurement::V_in + " V";
    doc["voltage_out"] = (String)edugrid_measurement::V_out + " V";
    doc["power_in"] = (String)edugrid_measurement::P_in + " W";
    doc["power_out"] = (String)edugrid_measurement::P_out + " W";
    doc["efficiency"] = (String)edugrid_measurement::efficiency + " %";

    serializeJson(doc, JSON_str);
    webSocket.broadcastTXT(JSON_str);
    JSON_str = "";
    doc.clear();
}

/** Handles uploads to the LittleFS file system (directory /log/ ) from webserver
 *
 */
void edugrid_webserver::handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    Serial.println(logmessage);

    if (!index)
    {
        logmessage = "Upload Start: " + String(filename);
        // open the file on first call and store the file handle in the request object
        request->_tempFile = LittleFS.open("/" + filename, "w");
        Serial.println(logmessage);
    }

    if (len)
    {
        // stream the incoming chunk to the opened file
        request->_tempFile.write(data, len);
        logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
        Serial.println(logmessage);
    }

    if (final)
    {
        logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
        // close the file handle as the upload is now done
        request->_tempFile.close();
        Serial.println(logmessage);
        request->redirect("/file");
    }
}

/** Returns a string with all files in LittleFs system
 * @return String with all files, HTML-format
 */
String edugrid_webserver::listFiles(bool ishtml)
{
    String returnText = "";
    Serial.println("| OK | Listing files stored on /LittleFs/log/");
    File root = LittleFS.open("/log/");
    File foundfile = root.openNextFile();

    returnText += "<table><tr><th align='left'>Name</th><th align='left'>Size</th><th></th><th></th></tr>";

    while (foundfile)
    {
        returnText += "<tr align='left'><td>" + String("/log/") + String(foundfile.name()) + "</td><td>" + edugrid_webserver::humanReadableSize(foundfile.size()) + "</td>";
        returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String("/log/") + String(foundfile.name()) + "\', \'download\')\">Download</button>";
        returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String("/log/") + String(foundfile.name()) + "\', \'delete\')\">Delete</button></tr>";

        foundfile = root.openNextFile();
    }
    root.close();
    foundfile.close();

    Serial.println("| OK | Listing files stored on /LittleFs/www/");
    root = LittleFS.open("/www/");
    foundfile = root.openNextFile();
    while (foundfile)
    {
        returnText += "<tr align='left'><td>" + String("/www/") + String(foundfile.name()) + "</td><td>" + edugrid_webserver::humanReadableSize(foundfile.size()) + "</td>";
        returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String("/www/") + String(foundfile.name()) + "\', \'download\')\">Download</button>";
        returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String("/www/") + String(foundfile.name()) + "\', \'delete\')\">Delete</button></tr>";

        foundfile = root.openNextFile();
    }
    root.close();
    foundfile.close();

    Serial.println("| OK | Listing files stored on /LittleFs/config");
    root = LittleFS.open("/config/");
    foundfile = root.openNextFile();
    while (foundfile)
    {
        returnText += "<tr align='left'><td>" + String("/config/") + String(foundfile.name()) + "</td><td>" + edugrid_webserver::humanReadableSize(foundfile.size()) + "</td>";
        returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String("/config/") + String(foundfile.name()) + "\', \'download\')\">Download</button>";
        returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String("/config/") + String(foundfile.name()) + "\', \'delete\')\">Delete</button></tr>";

        foundfile = root.openNextFile();
    }
    root.close();
    foundfile.close();

    Serial.println("| OK | Listing files stored on /LittleFs/");
    root = LittleFS.open("/");
    foundfile = root.openNextFile();
    while (foundfile)
    {
        returnText += "<tr align='left'><td>" + String("") + String(foundfile.name()) + "</td><td>" + edugrid_webserver::humanReadableSize(foundfile.size()) + "</td>";
        returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String("") + String(foundfile.name()) + "\', \'download\')\">Download</button>";
        returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String("") + String(foundfile.name()) + "\', \'delete\')\">Delete</button></tr>";

        foundfile = root.openNextFile();
    }
    root.close();
    foundfile.close();

    returnText += "</table>";
    return returnText;
}

String edugrid_webserver::humanReadableSize(const size_t bytes)
{
    if (bytes < 1024)
        return String(bytes) + " B";
    else if (bytes < (1024 * 1024))
        return String(bytes / 1024.0) + " KB";
    else if (bytes < (1024 * 1024 * 1024))
        return String(bytes / 1024.0 / 1024.0) + " MB";
    else
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}
