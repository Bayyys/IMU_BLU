#include <Arduino.h>
#include <AsyncTimer.h>
#include <BLEDevice.h>
#define USE_MULTCORE 1  // 0: 单核; 1: 多核

#include <vector>

using namespace std;

AsyncTimer t;
BLEScan* pBLEScan;
int is_scan = 1;
int doCon = 0;
vector<BLEAdvertisedDevice> device_list;
BLEAddress device_address = BLEAddress("cf:88:10:cb:0a:2e");
vector<BLEAddress> device_address_list;
vector<string> device_address_str_list = {
    "CF:88:10:CB:0A:2E", "F3:8C:9A:CF:F0:D7", "D9:69:71:72:2D:1E",
    "FF:E6:12:3B:08:A6", "E5:4B:A7:C7:85:32",
};
vector<long long> device_received_list = {0, 0, 0, 0, 0};

void initDeviceAddressList() {
  for (auto address_str : device_address_str_list) {
    device_address_list.push_back(BLEAddress(address_str));
  }
  t.setInterval(
      []() {
        for (auto num : device_received_list) {
          Serial.print(num);
          Serial.print(" ");
        }
        Serial.println();
      },
      1000);
}

// 客户端连接/断开回调
class MyClientCallbacks : public BLEClientCallbacks {
  void onConnect(BLEClient* pClient) {
    Serial.print(pClient->getPeerAddress().toString().c_str());
    Serial.println(" connected to the server!");
  }

  void onDisconnect(BLEClient* pClient) {
    Serial.print(pClient->getPeerAddress().toString().c_str());
    Serial.println(" disconnected from the server!");
  }
};

class MyClient {
 private:
  BLEClient* pClient;
  BLERemoteService* pService;
  // 指定 0000FFE4-0000-1000-8000-00805F9A34FB 为 Notify 特征
  BLERemoteCharacteristic* pNotifyChar;
  int BLEIndex = 0;

 public:
  MyClient(int index) { BLEIndex = index; }
  bool connect(BLEAdvertisedDevice device) {
    pClient = BLEDevice::createClient();
    BLEAdvertisedDevice* pServer = new BLEAdvertisedDevice(device);
    pClient->setClientCallbacks(new MyClientCallbacks());
    pClient->connect(pServer);
    if (pClient->isConnected()) {
      Serial.println("连接设备成功");
    } else {
      Serial.println("连接设备失败");
      return false;
    }

    pService = pClient->getService("0000ffe5-0000-1000-8000-00805f9a34fb");
    if (pService == nullptr) {
      Serial.println("获取服务失败");
      return false;
    }
    Serial.println("获取服务成功");

    pNotifyChar =
        pService->getCharacteristic("0000FFE4-0000-1000-8000-00805F9A34FB");
    if (pNotifyChar == nullptr) {
      Serial.println("获取特征失败");
      return false;
    } else {
      Serial.println("获取特征成功");
    }

    if (pNotifyChar->canNotify()) {
      pNotifyChar->registerForNotify(
          [&](BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData,
              size_t length,
              bool isNotify) { device_received_list[BLEIndex] += 1; });
    } else {
      Serial.println("不支持 Notify");
    }
    return true;
  }
};

// 扫描设备回调
class MyAdveertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // 判断名字是否以 "WT901" 开头
    if (advertisedDevice.getName().find("WT901") != std::string::npos) {
      for (auto device : device_list) {
        if (device.getAddress() == advertisedDevice.getAddress()) {
          return;
        }
      }
      // 如果设备列表中没有该设备, 则添加到设备列表中
      device_list.push_back(advertisedDevice);
      if (advertisedDevice.getAddress() == device_address) {
        Serial.println("Find the device!");
      }
      Serial.printf("Advertised Device: %s \n",
                    advertisedDevice.toString().c_str());
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
  initDeviceAddressList();
}

void connectToDevices(BLEAdvertisedDevice device, int index) {
  MyClient client = MyClient(index);
  if (client.connect(device)) {
    Serial.println("Connect successfully!");
  } else {
    Serial.println("Connect failed!");
  }
}

void loop() {
  if (doCon == 1) {
    Serial.println("Connecting...");
    for (int i = 0; i < device_list.size(); i++) {
      connectToDevices(device_list[i], i);
    }
    doCon = 2;
  }
  t.handle();
}

void serialEvent() {
  String str = Serial.readString();  // 读取串口数据
  Serial.print(str);
  // 去除换行符、回车及空格
  str.trim();
  if (str == "stop") {
    is_scan = 0;
  } else if (str == "scan") {
    is_scan = 1;
  } else if (str == "con") {
    if (doCon == 0) {
      Serial.println("识别到 con 指令");
      doCon = 1;
    }
  } else {
    Serial.println("Other");
  }
}