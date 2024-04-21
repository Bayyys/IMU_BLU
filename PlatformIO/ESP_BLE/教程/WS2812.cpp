#include <Arduino.h>
#include <NeoPixelBus.h>
#include <myInit.h>
#define shine 50

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(
    1, 38);  // 说明: 第一个参数是灯珠数量，第二个参数是引脚号

void setup() {
  // put your setup code here, to run once:
  myInit();

  strip.Begin();
  strip.Show();
}

void loop() {
  // put your main code here, to run repeatedly:
  // 依次点亮 红色 绿色 蓝色
  for (int i = 0; i < 3; i++) {
    strip.SetPixelColor(
        0, RgbColor(shine * (i == 0), shine * (i == 1), shine * (i == 2)));
    strip.Show();
    delay(500);
  }
}