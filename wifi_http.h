#ifndef WIFI_HTTP_H
#define WIFI_HTTP_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "sensors.h"

extern const char *ssid;
extern const char *pass;
extern const char *server_url;
extern const char *server_url_fire;

void initWiFi();
void send_data(const SensorData &data);
String check_fire(const SensorData &data);
String send_request(const char *endpoint, const char *method, String payload);

#endif
