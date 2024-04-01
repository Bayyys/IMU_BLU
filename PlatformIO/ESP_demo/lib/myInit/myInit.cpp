#include <Arduino.h>
#include <myInit.h>

void myInit() {
  Serial.begin(115200);
  Serial2.begin(115200);  // IO19: RX, IO18: TX
  pinMode(led, OUTPUT);
}