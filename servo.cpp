#include "servo.h"

Servo servo_base;
Servo servo_neck;

void initServo() {
  servo_base.attach(servo_base_pin);
  servo_neck.attach(servo_neck_pin);
}

void moveServo(int neck_pos, int position, const String& servo_cfg) {
  if (servo_cfg == "partial") {
    servo_neck.write(neck_pos);
    servo_base.write(position); 
  } else if (servo_cfg == "full") {
    servo_neck.write(position);
    servo_base.write(position); 
  }
}