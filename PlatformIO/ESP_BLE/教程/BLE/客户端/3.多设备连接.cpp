#include <main.h>

// 初始化设备地址列表 + 初始化定时器
void initDeviceAddressList() {
  for (auto address_str : device_address_str_list) {
    BLE_address_list.push_back(BLEAddress(address_str));
  }
  t_name = t.setInterval(
      []() {
        if (!doPrint) return;  // 如果不输出数据, 则直接返回
        for (auto update : device_data_update) {
          if (update == 0) {
            return;
          }
        }
        if (doDebug) {
          for (int i = 0; i < device_data.size(); i++) {
            Serial.printf("设备-%d: Roll: %.2f, Pitch: %.2f, Yaw: %.2f\n", i,
                          device_data[i][0], device_data[i][1],
                          device_data[i][2]);
          }
        } else {
          for (int i = 0; i < dData.size(); i++) {
            Serial.printf("%02x ", dData[i]);
          }
          Serial.println();
        }
        device_data_update.assign(device_data_update.size(), 0);
      },
      10);
  for (int i = 0; i < device_address_str_list.size(); i++) {
    for (int j = 0; j < 6; j++) {
      dData.push_back(0x00);
    }
  }
}

void scan_handler() {
  Serial.print("[scan_handler] 开始扫描, 未连接设备列表: ");
  for (int i = 0; i < BLE_address_list.size(); i++) {
    if (is_connected_device_list[i] == 0) {
      Serial.printf("%d ", i);
    }
  }
  Serial.println(".");
  pBLEScan->clearResults();  // 清空搜索结果
  pBLEScan->start(5);        // 开始搜索
}

void con_handler() {
  Serial.printf("[scan_handler] 连接至设备-%d\n", pConIndex);
  MyClient client = client_list[pConIndex];
  if (client.connect()) {
    Serial.printf("设备-%d Connect successfully!\n", pConIndex);
    is_connected_device_list[pConIndex] = 1;
  } else {
    Serial.printf("设备-%d Connect failed!\n", pConIndex);
  }
  doCon = 0;
  doScan = 1;
}

// 扫描设备回调
void MyAdveertisedDeviceCallbacks::onResult(
    BLEAdvertisedDevice advertisedDevice) {
  // 判断名字是否以 "WT901" 开头
  if (advertisedDevice.getName().find("WT901") != std::string::npos) {
    Serial.printf("[MyAdveertisedDeviceCallbacks] 扫描到设备: %s\n",
                  advertisedDevice.getAddress().toString().c_str());
    // 判断是否在连接列表中
    for (int i = 0; i < BLE_address_list.size(); i++) {
      // 扫描到设备且未连接
      if (advertisedDevice.getAddress() == BLE_address_list[i] &&
          is_connected_device_list[i] == 0) {
        Serial.printf("[MyAdveertisedDeviceCallbacks] 扫描可连接设备-%d: %s\n",
                      i, advertisedDevice.getAddress().toString().c_str());
        pDevice = new BLEAdvertisedDevice(advertisedDevice);
        pConIndex = i;
        pBLEScan->stop();  // 停止扫描
        doScan = 0;        // 停止扫描
        doCon = 1;         // 连接设备
        break;
      }
    }
  }
}

// 客户端连接/断开回调
MyClientCallbacks::MyClientCallbacks(int idx) { client_index_ = idx; };
void MyClientCallbacks::onConnect(BLEClient* pClient) {
  Serial.print(pClient->getPeerAddress().toString().c_str());
  Serial.println(" 加入连接");
  is_scan_down += 1;
  if (is_scan_down == BLE_address_list.size()) {
    Serial.println("-------所有设备已连接---------");
    if (!doPrint) {
      doPrint = 1;
      Serial.println("-------开始输出数据---------");
    }
  }
}
void MyClientCallbacks::onDisconnect(BLEClient* pClient) {
  is_connected_device_list[client_index_] = 0;  // 标志位清零
  Serial.print(pClient->getPeerAddress().toString().c_str());
  Serial.println(" 断开连接");
  is_scan_down -= 1;
  doPrint = 0;
  Serial.printf("----- 设备-%d 断开连接 -----\n", client_index_);
  Serial.printf("----- 剩余设备数: %d ----\n", is_scan_down);
}

// 连接设备
MyClient::MyClient() {
  BLEIndex_ = 0;
  pClient_ = BLEDevice::createClient();
  pService_ = nullptr;
  pNotifyChar_ = nullptr;
  pWriteChar_ = nullptr;
}
MyClient::MyClient(int idx) {
  BLEIndex_ = idx;
  pClient_ = BLEDevice::createClient();
  pService_ = nullptr;
  pNotifyChar_ = nullptr;
  pWriteChar_ = nullptr;
}
bool MyClient::connect() {
  Serial.printf("设备-%d 正在连接...\n", BLEIndex_);
  pClient_->setClientCallbacks(new MyClientCallbacks(BLEIndex_));
  pClient_->connect(pDevice);
  if (pClient_->isConnected()) {
    Serial.printf("设备-%d 连接成功\n", BLEIndex_);
  } else {
    Serial.printf("设备-%d 连接失败\n", BLEIndex_);
    return false;
  }

  pService_ = pClient_->getService("0000ffe5-0000-1000-8000-00805f9a34fb");
  if (pService_ == nullptr) {
    Serial.printf("设备-%d 获取服务失败\n", BLEIndex_);
    return false;
  }
  Serial.printf("设备-%d 获取服务成功\n", BLEIndex_);

  pNotifyChar_ =
      pService_->getCharacteristic("0000FFE4-0000-1000-8000-00805F9A34FB");
  if (pNotifyChar_ && pNotifyChar_->canNotify()) {
    Serial.printf("设备-%d 获取 Notify 特征成功\n", BLEIndex_);
  } else {
    Serial.printf("设备-%d 获取 Notify 特征失败\n", BLEIndex_);
    return false;
  }

  pWriteChar_ =
      pService_->getCharacteristic("0000FFE9-0000-1000-8000-00805F9A34FB");
  if (pWriteChar_ && pWriteChar_->canWriteNoResponse()) {
    pWriteCharList[BLEIndex_] = pWriteChar_;
    Serial.printf("设备-%d 获取 WriteNoRes 特征成功\n", BLEIndex_);
  } else {
    Serial.printf("设备-%d 获取 WriteNoRes 特征失败\n", BLEIndex_);
    return false;
  }

  auto idx = BLEIndex_;

  auto myNotifyCallback = [idx](
                              BLERemoteCharacteristic* pBLERemoteCharacteristic,
                              uint8_t* pData, size_t length, bool isNotify) {
    // string str = string((char*)pData);
    // 示例数据: bytes: 0x55 0x61 0x5A 0x00 0xBE 0xFF 0x37 0xF8 0x00 0x00
    // 0x00 0x00 0x00 0x00 0x72 0x82 0x03 0xFD 0x19 0xB8
    // 数据包头: 0x55
    // 标志位: 0x61
    // 加速度 角速度 角度

    if (!doPrint) return;
    float Roll = 0.0, Pitch = 0.0, Yaw = 0.0;
    while (*pData != 0x55) {  // 找到数据包头
      pData++;
      length--;
      if (length == 0) return;
    }
    if (length < 20) return;
    if (*(pData + 1) == 0x61) {
      // 滚转角(x轴) Roll = ((RolH << 8) | RolL) / 32768 * 180 (°)
      // 俯仰角(y轴) Pitch = ((PitchH << 8) | PitchL) / 32768 * 180 (°)
      // 偏航角(z轴) Yaw = ((YawH << 8) | YawL) / 32768 * 180 (°)
      if (doDebug) {
        Roll = (float)((short)(pData[15] << 8) | (short)pData[14]) / 32768.0f *
               180.0f;
        Pitch =
            (float)((short)(pData[17] << 8) | (short)pData[16]) / 32768.0 * 180;
        Yaw =
            (float)((short)(pData[19] << 8) | (short)pData[18]) / 32768.0 * 180;
        device_data[idx] = {Roll, Pitch, Yaw};
      }
      int dataIdx = idx * 6 + 2;
      for (int i = 0; i < 6; i++) {
        dData[dataIdx + i] = pData[14 + i];
      }
    }
    // device_data_list[idx]++;
    device_data_update[idx] = 1;
  };

  if (pNotifyChar_->canNotify()) {
    pNotifyChar_->registerForNotify(myNotifyCallback);
    Serial.printf("设备-%d 此特征值 支持 Notify\n", BLEIndex_);
  } else {
    Serial.printf("设备-%d 此特征值 不支持 Notify\n", BLEIndex_);
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Device started!");

  BLEDevice::init("ESP32_Bayyy");  // 初始化BLE设备, 其中的参数为设备名称
  pBLEScan = BLEDevice::getScan();  // 获取扫描对象
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdveertisedDeviceCallbacks(),
                                         true);  // 设置回调
  pBLEScan->setActiveScan(true);                 // 设置为主动扫描
  pBLEScan->setInterval(100);                    // 设置扫描间隔(ms)
  pBLEScan->setWindow(80);                       // 设置扫描窗口(ms)
  initDeviceAddressList();
  for (int i = 0; i < device_address_str_list.size(); i++) {
    MyClient client = MyClient(i);
    client_list.push_back(client);
  }
}

void loop() {
  t.handle();
  if (doScan) {
    if (is_scan_down < BLE_address_list.size()) {
      scan_handler();
    }
  }
  if (doCon) {
    con_handler();
  }
}

void serialEvent() {
  String str = Serial.readString();  // 读取串口数据
  Serial.print(str);
  // 去除换行符、回车及空格
  str.trim();
  if (str == "stop") {
    doScan = 0;
  } else if (str == "scan") {
    doScan = 1;
  } else if (str == "print") {
    doPrint = doPrint == 0 ? 1 : 0;
  } else if (str.startsWith("rate")) {
    // 如果是: "ratex" -> 更改速率为 第x个
    if (str.length() < 5) {
      return;
    }
    int rate = str.substring(4).toInt();
    if (rate >= 0 && rate < changeRate.size() - 1) {
      uint8_t* data = changeRate[rate].data();
      Serial.printf("更改速率为: %.1f Hz\n", rateHz[rate]);
      t.changeDelay(t_name, timerMs[rate]);  // 更改定时器时间
      Serial.printf("写入: %x %x %x %x %x\n", data[0], data[1], data[2],
                    data[3], data[4]);
      // 遍历已连接设备, pWriteChar 写入数据
      for (int i = 0; i < client_list.size(); i++) {
        if (is_connected_device_list[i] == 1) {
          if (pWriteCharList[i] != nullptr) {
            pWriteCharList[i]->writeValue(data, 5);
            Serial.printf("设备-%d 更改速率成功\n", i);
          }
        }
      }
    }
  } else if (str == "debug") {
    doDebug = doDebug == 0 ? 1 : 0;
    Serial.printf("调试模式: %s\n", doDebug ? "开启" : "关闭");
  } else {
    Serial.println("Other");
  }
}