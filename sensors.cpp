#include "sensors.h"

DHT dht(DHTPIN, DHTTYPE);
MQ135 mq135_sensor(MQ135_PIN);

bool compare(const SensorData& data1, const SensorData& data2) {
    return (
        data1.temperature != data2.temperature ||
        data1.humidity != data2.humidity ||
        data1.IR_value != data2.IR_value ||
        data1.mq135_rzero != data2.mq135_rzero ||
        data1.corrected_rzero != data2.corrected_rzero ||
        data1.resistance != data2.resistance ||
        data1.ppm != data2.ppm ||
        data1.corrected_ppm != data2.corrected_ppm
    );
}

void copy(SensorData& dest, const SensorData& src) {
    dest.temperature = src.temperature;
    dest.humidity = src.humidity;
    dest.IR_value = src.IR_value;
    dest.mq135_rzero = src.mq135_rzero;
    dest.corrected_rzero = src.corrected_rzero;
    dest.resistance = src.resistance;
    dest.ppm = src.ppm;
    dest.corrected_ppm = src.corrected_ppm;
}

SensorData readSensorData() {
    SensorData data;

    // DHT11
    data.humidity = dht.readHumidity();
    data.temperature = dht.readTemperature();
    if (isnan(data.humidity) || isnan(data.temperature) || data.humidity > 110 || data.temperature > 110) {
        Serial.println("Failed to read from DHT!");
        return { -1, -1, -1, -1, -1, -1, -1, -1};
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
    tone(BUZZER_PIN, 500);
    delay(50);
    noTone(BUZZER_PIN);
    delay(50);
}

bool check_anomaly(const SensorData& data, const Config& config) {
    return (data.IR_value == 0 ||
            data.temperature > config.temp_thrsh ||
            data.humidity < config.hum_thrsh ||
            data.corrected_rzero < config.rzero_thrsh ||
            data.corrected_ppm > config.ppm_thrsh);
}

void printSensorData(const SensorData& data) {
    Serial.print("* IR sensor value: ");
    Serial.println(data.IR_value);

    Serial.println("--- DHT11 ---");
    Serial.print("- Temperature: ");
    Serial.println(data.temperature);
    Serial.print("- Humidity: ");
    Serial.println(data.humidity);

    Serial.println("--- MQ135 ---");
    Serial.print("- MQ135 RZero: ");
    Serial.println(data.mq135_rzero);
    Serial.print("- Corrected RZero: ");
    Serial.println(data.corrected_rzero);
    Serial.print("- Resistance: ");
    Serial.println(data.resistance);
    Serial.print("- PPM: ");
    Serial.print(data.ppm);
    Serial.println("ppm");
    Serial.print("- Corrected PPM: ");
    Serial.print(data.corrected_ppm);
    Serial.println("ppm");
    Serial.println("-------------");
}