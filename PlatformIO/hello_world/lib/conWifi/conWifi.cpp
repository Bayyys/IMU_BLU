// Wifi 连接测试
// AP 模式: 由ESP32创建热点，手机连接ESP32热点，手机浏览器输入
// STA 模式: ESP32连接路由器，手机浏览器输入路由器分配的IP地址
#include <Arduino.h>
#include <WiFi.h>
#include <conWifi.h>

const char* ssid = "BERC306";
const char* pwd = "jielvo2021";

bool wifiConnected() { return WiFi.status() == WL_CONNECTED; }

void wifiScan() {
  Serial.println("Scan Info: ");
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

void wifiCon() {
  WiFi.mode(WIFI_STA);
  // 断开之前的连接
  WiFi.disconnect();
  // 连接WiFi
  WiFi.begin(ssid, pwd);
  Serial.println("Connecting to WiFi...");
  delay(200);

  int scan_cnt = 1;
  while (WiFi.status() != WL_CONNECTED) {
    // wifiScan();
    Serial.print(".");
    delay(2000);
  }

  Serial.print("WiFi connected! ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}