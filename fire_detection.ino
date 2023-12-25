#include "wifi_http.h"
#include "servo.h"
#include "sensors.h"

void setup() {
  Serial.begin(9600);
  initWiFi();
  initServo();
}

void loop() {
  // readAndSendSensorData();

  SensorData data = readSensorData();
  printSensorData(data);

  send_data(data);

  if (check_anomaly(data)) {
    DynamicJsonDocument fire_reponse(512);
    String response = check_fire(data);
    DeserializationError error = deserializeJson(fire_reponse, response);

    // Check for parsing errors
    if (error) {
      Serial.print(F("JSON parsing failed! Error code: "));
      Serial.println(error.c_str());
      return;
    }

    if (fire_reponse["fire"] == 1) {
      Serial.println("Fire detected -> servo moving");
      moveServo(1);
      handleLEDAndBuzzer();
    } else if (fire_reponse["fire"] == 0) {
      Serial.println("No fire detected -> servo resting");
    }
  } else {
    moveServo(-1);
  }
}