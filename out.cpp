#include "out.h"

Servo servo_base;
Servo servo_neck;

void initServo() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  pinMode(LED_PIN_RED, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);

  servo_base.attach(SERVO_BASE_PIN);
  servo_neck.attach(SERVO_NECK_PIN);
}

void handleLED(const bool& fire, const bool& status) {
  if (fire == 0) {
    digitalWrite(LED_PIN_RED, LOW);
    digitalWrite(LED_PIN_GREEN, HIGH * status);
  } else if (fire == 1) {
    digitalWrite(LED_PIN_GREEN, LOW);
    digitalWrite(LED_PIN_RED, HIGH * status);
  }
}

void handleBuzzer(const int& status) {
  if (status == 1) {
    tone(BUZZER_PIN, 200);
  } else if (status == 0) {
    digitalWrite(BUZZER_PIN, HIGH);
  }
}

void moveServo(const int& neck_pos, const int& base_pos) {
  servo_neck.write(neck_pos);
  servo_base.write(base_pos);
}