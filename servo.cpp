#include "servo.h"

Servo servo_base;
Servo servo_neck;

void initServo() {
  servo_base.attach(servo_base_pin);
  servo_neck.attach(servo_neck_pin);
}

void moveServo() {
  for (int pos = 0; pos <= 180; pos += 1) {
    servo_neck.write(pos);
    servo_base.write(pos);
    delay(10);
  }

  for (int pos = 180; pos >= 0; pos -= 1) {
    servo_neck.write(pos);
    servo_base.write(pos);
    delay(10);
  }
}