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

void initSensors();
void handleLEDAndBuzzer();
void readAndSendSensorData();

#endif