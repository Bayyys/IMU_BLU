#include <Arduino.h>
#include <Ticker.h>  // Ticker: 软件定时器库

#define led1 1
#define led2 4

// 定义定时器对象
Ticker timer1;
Ticker timer2;

void toggle(int pin) { digitalWrite(pin, !digitalRead(pin)); }

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // 设置定时器
  timer1.attach(0.5, toggle, led1);  // [周期执行] 定时时间(s), 回调函数, 参数
  // timer1.attach_ms(500, toggle, led1);  // 定时时间(ms), 回调函数, 参数
  timer2.once(3, toggle, led2);  // [单次执行] 定时时间(s), 回调函数, 参数
  // timer2.once_ms(3000, toggle, led2);

  // detach() 取消定时器
  // active() 判断定时器是否激活
}

void loop() {}