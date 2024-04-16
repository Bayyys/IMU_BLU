// 串口通信
#include <Arduino.h>
#include <conWifi.h>
#include <myInit.h>

void setup() {
  myInit();
  Serial2.begin(115200);  // IO19: RX, IO18: TX
}

void loop() {
  if (Serial2.available()) {
    Serial.write(Serial2.read());
  }
  if (Serial.available()) {
    Serial2.write(Serial.read());
  }
}