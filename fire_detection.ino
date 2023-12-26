#include "wifi_http.h"
#include "servo.h"
#include "sensors.h"

int fireornot = -1;
float global_correctedRZero = -1;
float global_resistance = -1;
float global_correctedppm = -1;
float global_humidity = -1;
float global_temperature = -1;
float global_IR_value = -1;

bool check_difference(const SensorData& data) {
    return (data.IR_value != global_IR_value  ||
            data.temperature != global_temperature ||
            data.humidity != global_humidity ||
            data.corrected_rzero != global_correctedRZero ||
            data.corrected_ppm != global_correctedppm ||
            data.resistance != global_resistance);
}

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

  if (check_anomaly(data) && check_difference(data)) {
    global_correctedppm = data.corrected_ppm; 
    global_correctedRZero = data.corrected_rzero;
    global_humidity = data.humidity;
    global_IR_value = data.IR_value;
    global_resistance = data.resistance;
    global_temperature = data.temperature;

    String response;
    DynamicJsonDocument icf_response(256);
    while(icf_response["status"] != "ok") {
      response = init_check_fire(data);
      icf_response = parseJson(response, 256);
      Serial.println("Waiting for server to be ready!!!");
    }

    for (int i = 0; i <= 180; i+=60) {
      moveServo(i);

      String response = check_fire(data);
      DynamicJsonDocument fire_response = parseJson(response, 256);
      fireornot = fire_response["fire"];
      String fire = fire_response["fire"];
      String img_url = fire_response["url"];

      if (fire_response["fire"] == 1) {
        Serial.println("!!! Fire detected !!!");
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