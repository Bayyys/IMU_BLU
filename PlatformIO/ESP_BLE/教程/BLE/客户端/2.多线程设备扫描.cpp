#include <Arduino.h>
#include <BLEDevice.h>
#define USE_MULTCORE 1  // 0: 单核; 1: 多核

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

void scanTask(void* xTask) {
  while (1) {
    if (is_scan == 1) {
      device_list.clear();
      pBLEScan->clearResults();  // 清空扫描结果
      BLEScanResults foundDevices =
          pBLEScan->start(5);  // 开始扫描(0: 永久扫描, 直到调用stop)
      Serial.print("Devices found: ");
      Serial.println(device_list.size());
    }
    delay(5000);
  }
}

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
#if !USE_MULTCORE
  xTaskCreate(scanTask, "scanTask", 4096, NULL, 1, NULL);
#else
  xTaskCreatePinnedToCore(scanTask, "scanTask", 4096, NULL, 1, NULL, 1);
  // 任务函数, 任务名称, 栈大小, 传递给任务函数的参数, 优先级(0是最低优先级),
  // 任务句柄, 核心编号(0: 核心0, 1: 核心1)
  // xTaskCreate 和 xTaskCreatePinnedToCore 的区别在于后者可以指定任务运行的核心
#endif
}

void loop() {}

void serialEvent() {
  String str = Serial.readString();  // 读取串口数据
  Serial.print(str);
  // 去除换行符、回车及空格
  str.trim();
  if (str == "stop") {
    is_scan = 0;
  } else if (str == "scan") {
    is_scan = 1;
  } else {
    Serial.println("Other");
  }
}