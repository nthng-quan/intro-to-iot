#include "wifi_http.h"
#include "servo.h"
#include "sensors.h"

void setup() {
  Serial.begin(9600);
  initSensors();
  initServo();
  initWiFi();
}

void loop() {
  SensorData data = readSensorData();
  printSensorData(data);

  send_data(data);

  if (check_anomaly(data)) {
    String response = init_check_fire(data);
    DynamicJsonDocument icf_response = parseJson(response, 256);
    if (icf_response["status"] == "ok") {
      moveServo(-1);
      String response = check_fire(data);
      DynamicJsonDocument fire_response = parseJson(response, 256);

      if (fire_response["fire"] == 1) {
        Serial.println("!!! Fire detected !!!");
        handleLEDAndBuzzer();
      } else if (fire_response["fire"] == 0) {
        Serial.println("No fire detected -> servo resting");
        // moveServo(-1);
      }
    }
  }
}