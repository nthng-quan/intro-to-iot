#include "servo.h"

Servo servo_base;
Servo servo_neck;

void initServo() {
  servo_base.attach(servo_base_pin);
  servo_neck.attach(servo_neck_pin);
}

void moveServo(int position) {
  servo_neck.write(90);
  servo_base.write(position);
}