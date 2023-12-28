#ifndef WIFI_HTTP_H
#define WIFI_HTTP_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "sensors.h"

extern const char *ssid;
extern const char *pass;
extern const char *server_url;
extern const char *server_url_fire;
extern const char *web_url;

void initWiFi();
void send_data(const SensorData &data);
void send_notification(const SensorData& data, const int& fire, const String& img_url);
String init_check_fire(const SensorData &data);
String check_fire(const SensorData &data, const int& neck_pos, const int& base_pos);
Config get_config();
DynamicJsonDocument parseJson(const String& jsonString, size_t capacity);
String send_request(const char *endpoint, const char *method, String payload);

#endif
