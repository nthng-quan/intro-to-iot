#include "wifi_http.h"
#include "servo.h"
#include "sensors.h"

void setup() {
  Serial.begin(9600);
  initWiFi();
  initServo();
}

void loop() {
  handleLEDAndBuzzer();
  readAndSendSensorData();
  moveServo();
}