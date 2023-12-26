#include "wifi_http.h"
#include "servo.h"
#include "sensors.h"

void setup() {
  Serial.begin(9600);
  initSensors();
  initServo();
  initWiFi();
}

// void loop() {
//   SensorData data = readSensorData();
//   printSensorData(data);

//   send_data(data);

//   if (check_anomaly(data)) {
//     String response = init_check_fire(data);
//     DynamicJsonDocument icf_response = parseJson(response, 256);
//     if (icf_response["status"] == "ok") {
//       moveServo(-1);
//       String response = check_fire(data);
//       DynamicJsonDocument fire_response = parseJson(response, 256);

//       if (fire_response["fire"] == 1) {
//         Serial.println("!!! Fire detected !!!");
//         handleLEDAndBuzzer();
//       } else if (fire_response["fire"] == 0) {
//         Serial.println("No fire detected -> servo resting");
//         // moveServo(-1);
//       }
//     }
//   }
// }

void loop() {
  if 


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

    while(icf_response["status"] != "ok"){
      String response = init_check_fire(data);
      DynamicJsonDocument icf_response = parseJson(response, 256);
      Serial.println("Waiting for ICF response to be ready!!!");
    }

    for (int i = 0; i < 180; i+=60) {
      moveServo(i);

      String response = check_fire(data);
      DynamicJsonDocument fire_response = parseJson(response, 256);
      fireornot = fire_response["fire"];
      if (fire_response["fire"] == 1) {
        Serial.println("!!! Fire detected !!!");
        handleLEDAndBuzzer();
        send_notification(data,fire_response["fire"]);
        break;
      } 
      else if (fire_response["fire"] == 0) {
        Serial.println("No fire detected -> servo resting");
      }
      send_notification(data,fire_response["fire"]);
    }
  }
}