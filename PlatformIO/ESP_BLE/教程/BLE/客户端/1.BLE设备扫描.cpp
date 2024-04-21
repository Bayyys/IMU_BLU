#include <Arduino.h>
#include <BLEDevice.h>

#include <vector>

using namespace std;

BLEScan* pBLEScan;
int is_scan = 1;
vector<BLEAddress> device_list;
BLEAddress device_address = BLEAddress("cf:88:10:cb:0a:2e");

// 扫描设备回调
class MyAdveertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // 判断名字是否以 "WT901" 开头
    if (advertisedDevice.getName().find("WT901") != std::string::npos) {
      if (find(device_list.begin(), device_list.end(),
               advertisedDevice.getAddress()) == device_list.end()) {
        // 如果设备列表中没有该设备, 则添加到设备列表中
        device_list.push_back(advertisedDevice.getAddress());
        if (advertisedDevice.getAddress() == device_address) {
          Serial.println("Find the device!");
        }
        Serial.printf("Advertised Device: %s \n",
                      advertisedDevice.toString().c_str());
      }
    }
  }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Device started!");

  BLEDevice::init("ESP32_Bayyy");  // 初始化BLE设备, 其中的参数为设备名称
  pBLEScan = BLEDevice::getScan();  // 获取扫描对象
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdveertisedDeviceCallbacks(),
                                         true);  // 设置回调
  pBLEScan->setActiveScan(true);                 // 设置为主动扫描
  pBLEScan->setInterval(100);                    // 设置扫描间隔(ms)
  pBLEScan->setWindow(80);                       // 设置扫描窗口(ms)
}

void loop() {
  if (is_scan == 1) {
    // put your main code here, to run repeatedly:
    BLEScanResults foundDevices = pBLEScan->start(10, false);  // 开始扫描
    Serial.print("Devices found: ");
    Serial.println(device_list.size());
    pBLEScan->clearResults();  // 清空扫描结果
  }
  delay(2000);  // 延时2s
  device_list.clear();
}