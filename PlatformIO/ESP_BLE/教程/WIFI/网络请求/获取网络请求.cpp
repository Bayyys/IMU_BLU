// 获取网络请求 HTTPClient
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <conWifi.h>
#include <myInit.h>

String url = "https://restapi.amap.com/v3/weather/weatherInfo";
String city = "330400";
String key = "612ed28339bc4f2e6317e51e744711fc";

String sendWebReq() {
  // 创建 HTTPClient 对象
  HTTPClient http;

  // 发送 GET 请求
  http.begin(url + "?city=" + city + "&key=" + key);

  // 响应状态码
  int code = http.GET();
  Serial.printf("HTTP Code: %d\n", code);

  // 响应正文
  String res = http.getString();
  Serial.printf("HTTP Response: %s\n", res.c_str());

  // 关闭连接
  http.end();

  return res;
}

void jsonParse(String res) {
  // 创建 JSON 文档
  JsonDocument doc;

  // 解析 JSON 文档
  deserializeJson(doc, res);

  // 从 JSON 文档中提取数据
  float temp = doc["lives"][0]["temperature"].as<float>();
  String weather = doc["lives"][0]["weather"].as<String>();
  String city = doc["lives"][0]["city"].as<String>();
  float humidity = doc["lives"][0]["humidity"].as<float>();

  Serial.printf("City: %s\n", city.c_str());
  Serial.printf("Temperature: %.2lf\n", temp);
  Serial.printf("Weather: %s\n", weather.c_str());
  Serial.printf("Humidity: %.2lf\n", humidity);
}

void setup() {
  myInit();
  wifiCon();
  String res = sendWebReq();
  jsonParse(res);
}

void loop() {
  if (wifiConnected()) {
    digitalWrite(led, HIGH);
    delay(250);
    digitalWrite(led, LOW);
    delay(250);
  }
  delay(1000);
}