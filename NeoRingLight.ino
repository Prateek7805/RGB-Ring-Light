#include "QWiFiAP.h"
#include <Adafruit_NeoPixel.h>
#include <driver/ledc.h>

#define LED_PIN 2
#define NUM_LEDS 24
#define LED_EN_PIN 1
#define LED_TYPE NEO_GRB + NEO_KHZ800

Adafruit_NeoPixel led_line = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, LED_TYPE);

QWiFiAP q_server(80);
int32_t strToHex(String hex)
{
  char *endptr;
  uint32_t value = (uint32_t)strtol(hex.c_str(), &endptr, 16); // string, end of conversion pointer, base of int conversion
  if (*endptr != '\0')
  {
    return -1;
  }
  return value;
}
int8_t strToU8(String dec)
{
  char *endptr;
  uint8_t value = (uint8_t)strtol(dec.c_str(), &endptr, 10); // string, end of conversion pointer, base of int conversion
  if (*endptr != '\0')
  {
    return -1;
  }
  return value;
}

struct led_vals {
  bool power = true;
  uint32_t color = 0;
  uint8_t brightness = 0;
  uint32_t _t = 0;
} led;

void led_power(bool power){
  pinMode(LED_EN_PIN, power? OUTPUT: INPUT);
  if(led.power){
    digitalWrite(LED_EN_PIN, LOW);
  }
}
void save_led_data() {
  if (!LittleFS.begin()) {
    Serial.println("[save_led_data] : LittleFS init error");
    return;
  }
  File file = LittleFS.open("/led.dat", "w");
  if (!file) {
    Serial.println("[save_led_data] : Failed to write LED Config file");
    return;
  }
  file.printf("%d\n%x\n%d\n", led.power, led.color, led.brightness);
  file.close();
}

void load_led_data() {
  if (!LittleFS.begin()) {
    Serial.println("[load_led_data] : LittleFS init error");
    return;
  }
  File file = LittleFS.open("/led.dat", "r");
  if (!file) {
    Serial.println("[load_led_data] : Failed to write LED Config file");
    return;
  }
  
  String power_s = file.readStringUntil('\n');
  int8_t power = strToU8(power_s);
  if(power != -1){
    led.power = (bool)power;
    led_power(led.power);  
  }
  String color_s = file.readStringUntil('\n');
  int32_t color = strToHex(color_s);
  if (color != -1) {
    led.color = color;
  }

  String bright_s = file.readStringUntil('\n');
  int8_t bright = strToU8(bright_s);
  if (bright != -1) {
    led.brightness = bright;
  }

  file.close();
}

void auto_save() {
  if (millis() - led._t > AUTO_SAVE_T) {
    save_led_data();
    led._t = millis();
  }
}
void setup() {
  Serial.begin(115200);
  //power on LED
  pinMode(LED_EN_PIN, OUTPUT);
  digitalWrite(LED_EN_PIN, LOW);
  load_led_data();
  delay(5000);

  q_server.on("/init", HTTP_GET, [](AsyncWebServerRequest * req) {
    String resJSON = "{\"power\":"+String(led.power)+
                     ",\"color\":\"#" + String(led.color, HEX) +
                     "\",\"bright\":" + String(led.brightness) + "}";
    req->send(200, "application/json", resJSON);
  });
  q_server.on("/power", HTTP_GET, [](AsyncWebServerRequest * req) {
    if (!req->hasParam("value")) {
      req->send(400, "text/plain", "Invalid parameter");
      return;
    }
    String power_s = req->getParam("value")->value();
    bool power = power_s == "y";
    led.power = power;
    led_power(power);
    req->send(200, "text/plain", "");
  });
  q_server.on("/color", HTTP_GET, [](AsyncWebServerRequest * req) {
    if (!req->hasParam("value")) {
      req->send(400, "text/plain", "Invalid parameter");
      return;
    }
    String color_s = req->getParam("value")->value();
    int32_t color = strToHex(color_s);
    if (color == -1) {
      Serial.println("[/color] : color paramter value error");
      req->send(400, "text/plain", "Invalid parameter value");
      return;
    }
    led.color = color;
    req->send(200, "text/plain", "");
  });
  q_server.on("/bright", HTTP_GET, [](AsyncWebServerRequest * req) {
    if (!req->hasParam("value")) {
      req->send(400, "text/plain", "Invalid parameter");
      return;
    }
    String bright_s = req->getParam("value")->value();
    int8_t bright = strToU8(bright_s);
    if (bright == -1) {
      Serial.println("[/color] : brightness paramter value error");
      req->send(400, "text/plain", "Invalid parameter value");
      return;
    }
    led.brightness = bright;
    req->send(200, "text/plain", "");
  });

  q_server.begin();
  led_line.begin();
  led_line.setBrightness(30);
}

void loop() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    led_line.setPixelColor(i, led_line.gamma32(led.color));
  }
  led_line.setBrightness(led.brightness);
  led_line.show();
  auto_save();
}
