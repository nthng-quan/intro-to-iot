#ifndef WIFI_HTTP_H
#define WIFI_HTTP_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

extern const char *ssid;
extern const char *pass;
extern const char* server_url;
extern const char* server_url_fire;

void initWiFi();
void send_data(
    int IR,
    float temperature,
    float humidity,
    float rzero,
    float correctedRZero,
    float resistance,
    float ppm,
    float correctedPPM
    );

String check_fire(
    int IR,
    float temperature,
    float humidity,
    float rzero,
    float correctedRZero,
    float resistance,
    float ppm,
    float correctedPPM
    );

String send_request(
    const char* endpoint, const char* method, String payload);

#endif
