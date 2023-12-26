#ifndef SERVO_H
#define SERVO_H

#include <Servo.h>

#define servo_neck_pin D3
#define servo_base_pin D5

extern Servo servo_base;
extern Servo servo_neck;

void initServo();
void moveServo(int neck_pos, int position, const String& servo_cfg);

#endif