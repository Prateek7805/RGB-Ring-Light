#ifndef _QWiFiAP_
#define _QWiFiAP_

#include "config.h"

#include <WiFi.h>
#include <esp_wifi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include "LittleFS.h"
#include "index.h"
#include "styles.h"
#include "script.h"


class QWiFiAP
{
private:
    AsyncWebServer *_ap_server = NULL;
    uint16_t _port = 80;
    struct wifiCreds
    {
        String ap_ssid = QWIFIAP_DEFAULT_AP_SSID;
        String ap_pass = QWIFIAP_DEFAULT_AP_PASS;
    } wc;

    bool _ap_server_definition(void);
    bool _load_FS(void);
    
    bool _parse_creds(uint8_t *data, String *ap_ssid, String *ap_pass);
    bool _validate_creds(String ap_ssid, String ap_pass, String *msg);
    bool _save_FS(void);
public:
    QWiFiAP();
    QWiFiAP(uint16_t port);
    void on(const char *uri, WebRequestMethodComposite method, ArRequestHandlerFunction handler);
    void begin(void);
    bool reset_credentials(void);
};

#endif _QWiFiAP_
