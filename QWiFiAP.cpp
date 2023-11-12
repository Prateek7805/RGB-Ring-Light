#include "QWiFiAP.h"

//*******************[Private Member Functions]**********************//

bool QWiFiAP::_parse_creds(uint8_t *data, String *ap_ssid, String *ap_pass)
{
    String body = (char *)data;

    // AP SSID
    int si = body.indexOf("\"ssid\":\"");
    if (si == -1)
    {
        return false;
    }
    si += 8;
    int ei = body.indexOf("\",", si);
    if (ei == -1)
    {
        return false;
    }
    *ap_ssid = body.substring(si, ei);

    // AP Password
    si = body.indexOf("\"pass\":\"");
    if (si == -1)
    {
        return false;
    }
    si += 8;
    ei = body.indexOf("\"", si);
    if (ei == -1)
    {
        return false;
    }
    *ap_pass = body.substring(si, ei);

    return true;
}

bool QWiFiAP::_validate_creds(String ap_ssid, String ap_pass, String *msg)
{
    if (ap_ssid.length() < 8 || ap_ssid.length() > 20 || ap_ssid.indexOf(" ") != -1)
    {
        *msg = "Invalid Device Name";
        return false;
    }

    if (ap_pass.length() < 8 || ap_pass.length() > 20)
    {
        *msg = "Invalid Device Password";
        return false;
    }

    return true;
}

bool QWiFiAP::_ap_server_definition(void)
{
    if (!_ap_server)
    {
        return false; // AP server initialization failed;
    }
    _ap_server->on("/", HTTP_GET, [](AsyncWebServerRequest *req)
                   { req->send_P(200, "text/html", _index); });
    _ap_server->on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *req)
                   { req->send_P(200, "text/css", _styles); });
    _ap_server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *req)
                   { req->send_P(200, "application/javascript", _script); });

    _ap_server->on("/reset", HTTP_GET, [this](AsyncWebServerRequest *req)
                   {
        if(!reset_credentials()){
            req->send(500, "text/plain", "[/reset] : Error in deleting credentials please restart the device");
            return;
        }
        req->send(200, "text/plain", "Credentials are resetted, please restart the device"); });
    _ap_server->on("/rmrf", HTTP_GET, [this](AsyncWebServerRequest *req)
                   {
        if (!LittleFS.begin())
        {
            Serial.println("[reset_credentials] : Failed to initialize LittleFS");
            req->send(500, "text/plain", "[reset_credentials] : Failed to initialize LittleFS");
            return;
        }
        if (!LittleFS.format())
        {
            Serial.println("[reset_credentials] : Failed to delete the saved credentials");
            req->send(500, "text/plain", "[reset_credentials] : Failed to delete the saved credentials");
            return;
        }
        req->send(200, "text/plain", "LittleFS Formatted!, please restart the device");
        ESP.restart(); });

    _ap_server->on(
        "/update", HTTP_POST, [](AsyncWebServerRequest *request)
        {
            AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError()) ? "Update Failed" : "Update Success");
            response->addHeader("Connection", "close");
            response->addHeader("Access-Control-Allow-Origin", "*");
            request->send(response); 
            ESP.restart();
        },
         [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            size_t content_len = request->contentLength();
            if (!index)
            {
                if (!Update.begin(content_len))
                {
                    Update.printError(Serial);
                }
            }
            if (Update.write(data, len) != len)
            {
                Update.printError(Serial);
            }
            else
            {
                Serial.printf("Progress: %d%%\n", (Update.progress() * 100) / Update.size());
            }
            if (final)
            {
                AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Please wait while the device reboots");
                response->addHeader("Refresh", "20");
                response->addHeader("Location", "/");
                request->send(response);
                if (!Update.end(true))
                {
                    Update.printError(Serial);
                }
                else
                {
                    Serial.println("Update complete");
                    Serial.flush();
                }
            }
        });
    // POST body handler for AP credential update
    _ap_server->onRequestBody([this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total)
                              {
        if (req->url() != "/creds"){
            req->send(400, "text/plain", "bad request");
            return;
        }
        if (len > 250){
            req->send(400, "text/plain", "body too large");
            return;
        }
        String dn,dp;
        
        if (!_parse_creds(data, &dn,&dp)){
            req->send(400, "text/plain", "error parsing json body");
            return;
        }
        String msg;
        if(!_validate_creds(dn,dp, &msg)){
            req->send(400, "text/plain", msg.c_str());
            return;
        }

        wc.ap_ssid = dn;
        wc.ap_pass = dp;
        
        if(!_save_FS()){
            req->send(500, "text/plain", "Failed to save credentials (～￣▽￣)～");
            return;
        }
        req->send(200, "text/plain", "Please restart the device to apply changes"); });
    return true; // AP server definition successful
}

bool QWiFiAP::_save_FS(void)
{
    if (!LittleFS.begin())
    {
        Serial.println("[_save_FS] : LittleFS check unsuccessful");
        Serial.println("[_save_FS] : Try restarting the device");
        return false;
    }
    File creds_file = LittleFS.open(CREDS_PATH, "w");
    if (!creds_file)
    {
        Serial.printf("[_save_FS] : Failed to create file at %s\n", CREDS_PATH);
        return false;
    }
    creds_file.printf("%s\n%s\n", wc.ap_ssid.c_str(), wc.ap_pass.c_str());
    creds_file.close();
    return true;
}

bool QWiFiAP::_load_FS(void)
{
    if (!LittleFS.begin())
    {
        Serial.println("[_load_FS] : LittleFS initialization unsuccessful");
        Serial.println("[_load_FS] : Resorting to default AP creds");
        return false;
    }
    if (!LittleFS.exists(CREDS_PATH))
    {
        Serial.printf("[_load_FS] : No file found at %s\n", CREDS_PATH);
        Serial.println("[_load_FS] : Resorting to default AP credentials");
        return false;
    }
    File creds_file = LittleFS.open(CREDS_PATH, "r");
    if (!creds_file)
    {
        Serial.printf("[_load_FS] : Failed to read credentials file at %s\n", CREDS_PATH);
        Serial.println("[_load_FS] : Resorting to default AP creds");
        return false;
    }
    wc.ap_ssid = creds_file.readStringUntil('\n');
    wc.ap_pass = creds_file.readStringUntil('\n');
    creds_file.close();
    return true;
}

//*******************[Public Member Functions]**********************//

// used for errors that can be fixed by restart

QWiFiAP::QWiFiAP(uint16_t port)
{
    _port = port;
    _ap_server = new AsyncWebServer(_port);
}

QWiFiAP::QWiFiAP()
{
    QWiFiAP(80);
}

bool QWiFiAP::reset_credentials()
{
    if (!LittleFS.begin())
    {
        Serial.println("[reset_credentials] : Failed to initialize LittleFS");
        return false;
    }
    if (!LittleFS.remove(CREDS_PATH))
    {
        Serial.println("[reset_credentials] : Failed to delete the saved credentials");
        return false;
    }
    return true;
}

void QWiFiAP::on(const char *uri, WebRequestMethodComposite method, ArRequestHandlerFunction handler)
{
    _ap_server->on(uri, method, handler);
}

void QWiFiAP::begin(void)
{
    if (!_load_FS())
    {
        Serial.println("[begin] : Failed to get Creds/data from FS");
    }
    if (!_ap_server_definition())
    {
        Serial.println("[begin] : AP server initialization failed");
        return;
    }
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(wc.ap_ssid.c_str(), wc.ap_pass.c_str());
    IPAddress AP_IP = WiFi.softAPIP();
    Serial.print("[begin] : ");
    Serial.println(AP_IP);
    _ap_server->begin();
}
