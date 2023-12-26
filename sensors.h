#ifndef SENSORS_H
#define SENSORS_H

#include <DHT.h>
#include <MQ135.h>

#define IR_PIN D2
#define BUZZER_PIN D1
#define LED_PIN D0
#define DHTPIN D7
#define DHTTYPE DHT11
#define MQ135_PIN A0

extern DHT dht;
extern MQ135 mq135_sensor;

// extern int fireornot;
// extern float global_correctedRZero;
// extern float global_resistance;
// extern float global_correctedppm;
// extern float global_humidity;
// extern float global_temperature;
// extern float global_IR_value;

struct SensorData {
    float temperature;
    float humidity;
    int IR_value;
    float mq135_rzero;
    float corrected_rzero;
    float resistance;
    float ppm;
    float corrected_ppm;
};

SensorData readSensorData();
void initSensors();
void handleLEDAndBuzzer();
bool check_anomaly(const SensorData& data);
// bool check_difference(const SensorData& data);
void printSensorData(const SensorData& data);

#endif