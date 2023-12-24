#include "sensors.h"
#include "wifi_http.h"

SensorData readSensorData() {
    SensorData data;

    // DHT11
    data.humidity = dht.readHumidity();
    data.temperature = dht.readTemperature();
    if (isnan(data.humidity) || isnan(data.temperature)) {
        Serial.println("Failed to read from DHT!");
        return data;
    }

    // IR sensor
    data.IR_value = digitalRead(IR_PIN);

    // MQ-135
    data.mq135_rzero = mq135_sensor.getRZero();
    data.corrected_rzero = mq135_sensor.getCorrectedRZero(data.temperature, data.humidity);
    data.resistance = mq135_sensor.getResistance();
    data.ppm = mq135_sensor.getPPM();
    data.corrected_ppm = mq135_sensor.getCorrectedPPM(data.temperature, data.humidity);

    return data;
}


DHT dht(DHTPIN, DHTTYPE);
MQ135 mq135_sensor(MQ135_PIN);

void initSensors() {
    pinMode(IR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    dht.begin();
}

void handleLEDAndBuzzer() {
    // LED
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);

    // Buzzer
    tone(BUZZER_PIN, 1000);
    delay(50);
    noTone(BUZZER_PIN);
    delay(50);
}

void readAndSendSensorData() {
    SensorData data = readSensorData();

    Serial.println("DHT11: ");
    Serial.print("- Temperature: ");
    Serial.println(data.temperature);
    Serial.print("- Humidity: ");
    Serial.println(data.humidity);

    Serial.print("IR sensor value: ");
    Serial.println(data.IR_value);

    Serial.println("---MQ135---");
    Serial.print("MQ135 RZero: ");
    Serial.println(data.mq135_rzero);
    Serial.print("Corrected RZero: ");
    Serial.println(data.corrected_rzero);
    Serial.print("Resistance: ");
    Serial.println(data.resistance);

    Serial.print("PPM: ");
    Serial.print(data.ppm);
    Serial.println("ppm");

    Serial.println("Corrected PPM: ");
    Serial.print(data.corrected_ppm);
    Serial.println("ppm");
    Serial.println("----------------");

    send_data(data);
    String fire_response = check_fire(data);
}