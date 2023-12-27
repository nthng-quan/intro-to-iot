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
  if (data.temperature == -1) {
    return;
  }
  Config config = get_config(data);
  // printSensorData(data);
  send_data(data);

  if (check_anomaly(data, config) && compare(global_data, data)) {
    String payload = String("{\n") +
            "    \"IR\": " + data.IR_value + ",\n" +
            "    \"MQ-135\": {\n" +
            "        \"correctedPPM\": " + data.corrected_ppm + ",\n" +
            "        \"correctedRZero\": " + data.corrected_rzero + ",\n" +
            "        \"ppm\": " + data.ppm + ",\n" +
            "        \"resistance\": " + data.resistance + ",\n" +
            "        \"rzero\": " + data.mq135_rzero + "\n" +
            "    },\n" +
            "    \"DHT\": {\n" +
            "        \"humidity\": " + data.humidity + ",\n" +
            "        \"temperature\": " + data.temperature + "\n" +
            "    }\n" +
            "}\n";
    send_request("http://192.168.1.5:5555/fire", "POST", payload);
    Serial.println("*** Abnormal data detected!!!");
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