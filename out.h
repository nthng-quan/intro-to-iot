#ifndef OUT_H
#define OUT_H

#include <Servo.h>

#define SERVO_NECK_PIN D3
#define SERVO_BASE_PIN D5
#define BUZZER_PIN D1
#define LED_PIN_RED D0
#define LED_PIN_GREEN D4

extern Servo servo_base;
extern Servo servo_neck;

void initServo();
void moveServo(const int& neck_pos, const int& base_pos);
void handleLED(const bool& fire, const bool& status);
void handleBuzzer(const int& status);

#endif