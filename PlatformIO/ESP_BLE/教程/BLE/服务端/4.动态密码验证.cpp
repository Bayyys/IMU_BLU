#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// 为BLE服务和特征定义UUID
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

bool isAdvertising = false;  // 是否正在广播
int clientCount = 0;         // 客户端数量

// 服务器回调
class MyServerCallbacks : public BLEServerCallbacks {
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

// 安全回调
class MySecurity : public BLESecurityCallbacks {
  uint32_t onPassKeyRequest() {  // 请求密码
    Serial.println("PassKeyRequest");
    return 123456;
  }

  bool onConfirmPIN(uint32_t pass_key) {  // 显示动态码并确认是否同意配对
    Serial.printf("The passkey YES/NO number: %d !", pass_key);
    return true;
  }

  void onAuthenticationComplete(esp_ble_auth_cmpl_t auth_cmpl) {  // 认证结果
    if (!auth_cmpl.success) {
      Serial.println("Authentication failed!");
    } else {
      Serial.println("Authentication success!");
    }
  }

  bool onSecurityRequest() {  // 安全请求
    Serial.println("SecurityRequest");
    return true;
  }

  void onPassKeyNotify(uint32_t pass_key) {  // 通知密码
    Serial.printf("The passkey Notify number: %d !\n", pass_key);
  }
};

void setup() {
  Serial.begin(115200);

  // 初始化串口
  BLEDevice::init("ESP32_Bayyy");
  BLEDevice::setEncryptionLevel(
      ESP_BLE_SEC_ENCRYPT_MITM);  // 设置加密级别(ESP_BLE_SEC_ENCRYPT_MITM:
                                  // 需要配对)
  BLEDevice::setSecurityCallbacks(new MySecurity());  // 设置安全回调
  auto local_address = BLEDevice::getAddress();
  Serial.print("Local Address: ");
  Serial.println(local_address.toString().c_str());

  // 创建BLE服务器
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());  // 设置服务器回调

  // 设置安全回调
  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(
      ESP_LE_AUTH_REQ_SC_MITM_BOND);        // 设置认证模式(需要配对)
  pSecurity->setCapability(ESP_IO_CAP_IO);  // 设置IO能力
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK |
                                  ESP_BLE_ID_KEY_MASK);  // 设置初始加密密钥

  // 创建一个 BLE 服务
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // 创建一个 BLE 特征
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
                               BLECharacteristic::PROPERTY_WRITE |
                               BLECharacteristic::PROPERTY_NOTIFY);
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
  if (BLEDevice::getInitialized() && !isAdvertising && clientCount <= 0) {
    BLEDevice::startAdvertising();  // 开始广播
    isAdvertising = true;
    Serial.println("Start advertising...");
  }
  delay(50);
}
