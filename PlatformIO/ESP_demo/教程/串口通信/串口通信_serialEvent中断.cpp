#include <Arduino.h>

using namespace std;

void setup() { Serial.begin(115200); }

void loop() {}

void serialEvent() {
  String str = Serial.readString();  // 读取串口数据
  Serial.print(str);
  // 去除换行符、回车及空格
  str.trim();
  if (str == "AT") {
    Serial.println("OK");
  } else {
    Serial.println("ERROR");
  }
}