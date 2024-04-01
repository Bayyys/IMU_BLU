// LEDC 产生 PWM 波
#include <Arduino.h>
#define FREQ 2000     // PWM 频率
#define CHANNEL 0     // PWM 通道
#define RESOLUTION 8  // PWM 分辨率
#define LED 1         // LED 引脚

void setup() {
  ledcSetup(CHANNEL, FREQ, RESOLUTION);  // 初始化 PWM
  ledcAttachPin(LED, CHANNEL);           // 将 PWM 通道绑定到引脚
}

void loop() {
  for (int i = 0; i < pow(2, RESOLUTION); i++) {
    ledcWrite(CHANNEL, i);  // 输出 PWM
    delay(10);
  }
  for (int i = pow(2, RESOLUTION) - 1; i > 0; i--) {
    ledcWrite(CHANNEL, i);
    delay(10);
  }
}