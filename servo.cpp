#include "servo.h"

Servo servo_base;
Servo servo_neck;

void initServo() {
  servo_base.attach(servo_base_pin);
  servo_neck.attach(servo_neck_pin);
}

// void moveServo(int neck_pos) {
//   for (int pos = 0; pos <= 90; pos += 1) {
//     if (neck_pos == -1) {
//       servo_neck.write(0);
//     } else {
//       servo_neck.write(pos);
//     }

//     servo_base.write(pos);
//     delay(5);
//   }

//   for (int pos = 90; pos >= 0; pos -= 1) {
//     if (neck_pos == -1) {
//       servo_neck.write(0);
//     } else {
//       servo_neck.write(pos);
//     }

//     servo_base.write(pos);
//     delay(5);
//   }
// }

void moveServo(int position) {
  servo_neck.write(90);
  servo_base.write(position);
}