// Wifi 连接测试
// AP 模式: 由ESP32创建热点，手机连接ESP32热点，手机浏览器输入
// STA 模式: ESP32连接路由器，手机浏览器输入路由器分配的IP地址
#include <Arduino.h>
#include <WiFi.h>
#define led 2

const char* ssid = "BERC306";
const char* pwd = "jielvo2021";

void wifiScan() {
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
}

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);

  WiFi.mode(WIFI_STA);
  // 断开之前的连接
  WiFi.disconnect();
  // 连接WiFi
  WiFi.begin(ssid, pwd);
  Serial.print("Connecting");
  delay(200);

  int scan_cnt = 1;

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("第" + String(scan_cnt) + "次扫描");
    wifiScan();
    delay(2000);
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  for (int i = 0; i < 5; i++) {
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
  }

  digitalWrite(led, HIGH);
  delay(5000);
  digitalWrite(led, LOW);
}

void loop() {}