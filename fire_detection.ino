#include "wifi_http.h"
#include "out.h"
#include "sensors.h"

SensorData global_data = {-1, -1, -1, -1, -1, -1, -1, -1};
Config global_config = {-1, -1, -1, -1, -1, -1};

unsigned long alarmTime = 0;

void setup() {
    Serial.begin(9600);
    initSensors();
    initServo();
    initWiFi();
}

void loop() {
    SensorData data = readSensorData();
    if (data.temperature == -1) {
        delay(1000);
        return;
    }

    Config config = get_config();

    if (compare(global_config, config, "servo")) {
        moveServo(config.servo_base, config.servo_neck);
        Serial.println("*** Servo config changed !!!");
        copy(global_config, config);
        send_capture();
    }

    // printSensorData(data);
    send_data(data);

    int fire_status;
    String img_url;

    if (check_anomaly(data, config) && compare(global_data, data, "dht")) {
        Serial.println("*** Abnormal data detected!!!");
        copy(global_data, data);

        DynamicJsonDocument icf_response(256);
        while (icf_response["status"] != "ok") {
            String response = init_check_fire(data);
            icf_response = parseJson(response, 256);
            Serial.println("Waiting for server to be ready!!!");
        }

        if (alarmTime == 0) alarmTime = millis();

        for (int i = 0; i <= 180; i += 60) {
            Serial.println("-> Rotate servo to " + String(i) + " degree");
            moveServo(i, config.servo_neck);
            delay(1000);

            String response = check_fire(data, i, config.servo_neck);
            DynamicJsonDocument fire_response = parseJson(response, 256);

            fire_status = (int) fire_response["fire"];
            img_url = (String) fire_response["url"];

            if (fire_status == 1) {
                Serial.println("[x] !!!! Fire detected !!!!");
                send_notification(data, fire_status, img_url);
                break;
            } else if (fire_status == 0) {
                handleLED(fire_status, 1);
                handleBuzzer(0);
                Serial.println("No fire detected here");
            }
        }
    } else { // reset alarm
        Serial.println("No abnormal data detected");
        alarmTime = 0;
        handleLED(0, 1);
        handleBuzzer(0);
        moveServo(config.servo_base, config.servo_neck);
    }

    Serial.println("--Fire start time: " + String(alarmTime));
    Serial.println("--Current time: " + String(millis()));

    if (alarmTime > 0) {
        if ((millis() - alarmTime) >= 20000) {
            alarmTime = 0;
            handleLED(fire_status, 0);
            handleBuzzer(0);
        } else {
            handleLED(fire_status, 1);
            handleBuzzer(fire_status);
        }
    }

    delay(1000);
}