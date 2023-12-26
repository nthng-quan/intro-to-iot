#include "wifi_http.h"
#include "servo.h"
#include "sensors.h"

int fireornot = -1;
SensorData global_data = { -1, -1, -1, -1, -1, -1, -1, -1 };

void setup() {
  Serial.begin(9600);
  initSensors();
  initServo();
  initWiFi();
}

void loop() {
  SensorData data = readSensorData();
  Config config = get_config(data);
  printSensorData(data);
  send_data(data);

  if (check_anomaly(data, config) && compare(global_data, data)) {
    copy(global_data, data);

    String response;
    DynamicJsonDocument icf_response(256);
    while(icf_response["status"] != "ok") {
      response = init_check_fire(data);
      icf_response = parseJson(response, 256);
      Serial.println("Waiting for server to be ready!!!");
    }

    for (int i = 0; i <= 180; i+=60) {
      moveServo(0, i, config.servo);

      String response = check_fire(data);
      DynamicJsonDocument fire_response = parseJson(response, 256);
      fireornot = fire_response["fire"];
      String fire = fire_response["fire"];
      String img_url = fire_response["url"];

      if (fire_response["fire"] == 1) {
        Serial.println("!!!! Fire detected !!!!");
        handleLEDAndBuzzer();
        send_notification(data, fire, img_url);
        break;
      } 
      else if (fire_response["fire"] == 0) {
        Serial.println("No fire detected -> servo resting");
      }
      send_notification(data, fire, img_url);
    }
  }
}