#include <Arduino.h>
#include <myInit.h>
#define USE_MULTCORE 1  // 0: 单核; 1: 多核

void xTaskOne(void *xTask1) {
  while (1) {
    Serial.println("Task 1 is running");
    delay(500);
  }
}

void xTaskTwo(void *xTask2) {
  while (1) {
    Serial.println("Task 2 is running");
    delay(1000);
  }
}

void setup() {
  // put your setup code here, to run once:
  myInit();
  delay(10);

// 创建两个任务
#if !USE_MULTCORE
  xTaskCreate(xTaskOne,  // 任务函数
              "Task1",   // 任务名称
              4096,      // 栈大小
              NULL,      // 传递给任务函数的参数
              1,         // 优先级(0是最低优先级)
              NULL);     // 任务句柄
  xTaskCreate(xTaskTwo, "Task2", 4096, NULL, 2, NULL);
#else
  xTaskCreatePinnedToCore(xTaskOne, "Task1", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(xTaskTwo, "Task2", 4096, NULL, 2, NULL, 1);
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  delay(5000);
}