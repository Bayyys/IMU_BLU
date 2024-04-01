#include <Arduino.h>
#include <AsyncTimer.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUUID.h>
#include <BLEUtils.h>

// 为BLE服务和特征定义UUID
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CNT_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"

bool isAdvertising = false;  // 是否正在广播
int clientCount = 0;         // 客户端数量
int cnt_value = 0;
BLECharacteristic *pChar = nullptr;
BLECharacteristic *pCharCnt = nullptr;
AsyncTimer t;

class MyServerCallbacks : public BLEServerCallbacks {  // 服务器回调
  void onConnect(BLEServer *pServer) {
    Serial.println("Device connected");
    clientCount++;
    isAdvertising = false;
  }

  void onDisconnect(BLEServer *pServer) {
    Serial.println("Device disconnected");
    clientCount--;
  }
};

void setup() {
  Serial.begin(115200);

  // 初始化串口
  BLEDevice::init("ESP32_Bayyy");
  auto local_address = BLEDevice::getAddress();
  Serial.print("Local Address: ");
  Serial.println(local_address.toString().c_str());

  // 创建BLE服务器
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());  // 设置服务器回调

  // 创建一个 BLE 服务
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // 创建一个 BLE 特征
  // 1. 读写特征值
  pChar = pService->createCharacteristic(
      CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  // 2. 周期广播特征值
  pCharCnt = pService->createCharacteristic(
      CHAR_CNT_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  // 给特征赋值
  pChar->setValue("Hello World! Normal Characteristic!");
  pCharCnt->setValue(cnt_value);
  // pCntCharacteristic->addDescriptor(
  //     new BLE2902());  // 添加描述符(启用Notify和Indicate需要添加)

  // BLEDescriptor *pCntName = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  // pCntName->setValue("My Counter");
  // pCntCharacteristic->addDescriptor(pCntName);

  // 开始服务
  pService->start();

  // 开启广播
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();  // 获取广播对象
  pAdvertising->addServiceUUID(SERVICE_UUID);  // 添加服务UUID
  pAdvertising->setScanResponse(true);         // 设置扫描响应
  pAdvertising->setMinPreferred(
      0x12);  // 此处设置值含义为：广播最小间隔(0x12*0.625ms=18.75ms)
  BLEDevice::startAdvertising();  // 开始广播
  Serial.println("Characteristic defined! Now you can read it in your phone!");

  t.setInterval(
      [] {
        cnt_value++;
        if (clientCount > 0) {
          pCharCnt->setValue(cnt_value);
          pCharCnt->notify();
        }
      },
      10);  // 10ms更新一次

  t.setInterval(
      [] {
        if (BLEDevice::getInitialized() && !isAdvertising && clientCount <= 0) {
          delay(500);                     // 延迟500ms
          BLEDevice::startAdvertising();  // 开始广播
          isAdvertising = true;
          Serial.println("Restart advertising...");
        }
      },
      50);  // 断线重连
}

void loop() {
  // 必须调用handle函数
  t.handle();
}
