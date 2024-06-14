#include <Arduino.h>

#include <vector>
using namespace std;

vector<uint8_t> v = {0, 0, 0, 0, 0};
int idx = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Device connected!");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  // 串口打印 v
  for (int i = 0; i < v.size(); i++) {
    if (i == idx) {
      v[i]++;
    }
  }
  Serial.write(v.data(), v.size());
  Serial.println();
  idx = (idx + 1) % v.size();
}
