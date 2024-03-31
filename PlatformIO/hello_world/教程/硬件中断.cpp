#include <Arduino.h>
#define led1 1
#define led2 4

hw_timer_t* timer1 = NULL;
hw_timer_t* timer2 = NULL;

void TOGGLE_LED1() { digitalWrite(led1, !digitalRead(led1)); }
void TOGGLE_LED2() { digitalWrite(led2, !digitalRead(led2)); }

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // 初始化定时器
  timer1 = timerBegin(0, 80, true);  // 定时器编号, 分频系数, 是否为向上计数
  timer2 = timerBegin(1, 80, true);

  // 配置定时器
  timerAttachInterrupt(
      timer1, &TOGGLE_LED1,
      true);  // 定时器指针, 中断处理函数, true: 边沿触发/false: 电平触发
  timerAttachInterrupt(timer2, &TOGGLE_LED2, true);

  // 设置定时模式
  timerAlarmWrite(timer1, 1000 * 1000,
                  true);  // 定时器指针, 定时时间, 是否自动重加载
  timerAlarmWrite(timer2, 1000 * 3000, false);

  // 启动定时器
  timerAlarmEnable(timer1);
  timerAlarmEnable(timer2);
}

void loop() {}