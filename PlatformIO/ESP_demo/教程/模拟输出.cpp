#include <Arduino.h>
#define led 1

void setup() { pinMode(led, OUTPUT); }

void loop() {
  for (int i = 0; i < 255; i++) {
    analogWrite(
        led,
        i);  // analogWrite() 用于模拟输出: pin: 引脚编号, value: 输出值(0~255)
    delay(10);
  }
  for (int i = 255; i > 0; i--) {
    analogWrite(led, i);
    delay(10);
  }
}