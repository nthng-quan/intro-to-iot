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

struct SensorData {
    int temperature;
    int humidity;
    int IR_value;
    float mq135_rzero;
    float corrected_rzero;
    float resistance;
    float ppm;
    float corrected_ppm;
};

struct Config {
    int servo_neck;
    int servo_base;
    float ppm_thrsh;
    float rzero_thrsh;
    int temp_thrsh;
    int hum_thrsh;
}; 

SensorData readSensorData();
bool compare(const SensorData& data1, const SensorData& data2);
void copy(SensorData& dest, const SensorData& src);

void initSensors();
void handleLEDAndBuzzer();
bool check_anomaly(const SensorData& data, const Config& config);
void printSensorData(const SensorData& data);

#endif