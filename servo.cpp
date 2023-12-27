#include "servo.h"

Servo servo_base;
Servo servo_neck;

void initServo() {
  servo_base.attach(servo_base_pin);
  servo_neck.attach(servo_neck_pin);
}

void moveServo(const int& neck_pos, const int& base_pos) {
  servo_neck.write(neck_pos);
  servo_base.write(base_pos);
}