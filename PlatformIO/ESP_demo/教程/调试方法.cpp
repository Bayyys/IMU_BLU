// 调试信息测试
// platformio.ini 配置: build_flags =	-DCORE_DEBUG_LEVEL=x
// x: 0(无信息), 1(错误信息), 2(警告信息), 3(信息), 4(调试信息), 5(重要信息)
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Device is ready!");
}

void loop() {
  delay(1000);
  // 无信息(0)
  log_n("This is an none message");
  // 错误信息(1)
  log_e("This is an error message");
  // 警告信息(2)
  log_w("This is a warning message");
  // 信息(3)
  log_i("This is an information message");
  // 调试信息(4)
  log_d("This is a debug message");
  // 重要信息(5)
  log_v("This is a verbose message");
}