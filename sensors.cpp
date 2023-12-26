#include "sensors.h"
#include "wifi_http.h"

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
    // DHT11
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT!");
        return;
    }
    
    Serial.println("DHT11: ");
    Serial.print("- Temperature: ");
    Serial.println(temperature);
    Serial.print("- Humidity: ");
    Serial.println(humidity);

    // IR sensor
    int IR_value = digitalRead(IR_PIN);
    Serial.print("IR sensor value: ");
    Serial.println(IR_value);


    // MQ-135
    float rzero = mq135_sensor.getRZero();
    float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
    float resistance = mq135_sensor.getResistance();
    float ppm = mq135_sensor.getPPM();
    float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);

    Serial.println("---MQ135---");
    Serial.print("MQ135 RZero: ");
    Serial.println(rzero);
    Serial.print("Corrected RZero: ");
    Serial.println(correctedRZero);
    Serial.print("Resistance: ");
    Serial.println(resistance);
    
    Serial.print("PPM: ");
    Serial.print(ppm);
    Serial.println("ppm");
    
    Serial.println("Corrected PPM: ");
    Serial.print(correctedPPM);
    Serial.println("ppm");
    Serial.println("----------------");

    send_data(IR_value, temperature, humidity, rzero, correctedRZero, resistance, ppm, correctedPPM);

    String fire_response = check_fire(IR_value, temperature, humidity, rzero, correctedRZero, resistance, ppm, correctedPPM);
    Serial.println(fire_response);
}
