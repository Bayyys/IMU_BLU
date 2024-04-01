#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// 为BLE服务和特征定义UUID
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

void setup() {
  Serial.begin(115200);

  // 初始化串口
  BLEDevice::init("ESP32_Bayyy");
  auto local_address = BLEDevice::getAddress();
  Serial.print("Local Address: ");
  Serial.println(local_address.toString().c_str());

  // 创建BLE服务器
  BLEServer *pServer = BLEDevice::createServer();
  // 创建一个 BLE 服务
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // 创建一个 BLE 特征
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  // 设置特征值
  pCharacteristic->setValue("Hello World Bayyy");
  // 开始服务
  pService->start();

  // 开启广播
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();  // 获取广播对象
  pAdvertising->addServiceUUID(SERVICE_UUID);  // 添加服务UUID
  pAdvertising->setScanResponse(true);         // 设置扫描响应
  pAdvertising->setMinPreferred(0x12);         // 设置最小首选值
  BLEDevice::startAdvertising();               // 开始广播
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  // 服务器代码在这里
}
