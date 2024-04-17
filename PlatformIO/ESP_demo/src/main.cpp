#include <main.h>

// 初始化设备地址列表 + 初始化定时器
void initDeviceAddressList() {
  for (auto address_str : device_address_str_list) {
    BLE_address_list.push_back(BLEAddress(address_str));
  }
  t.setInterval(
      []() {
        // Serial.printf("%d %d %d %d %d\n", dcnt1, dcnt2, dcnt3, dcnt4, dcnt5);
        for (auto data : device_data_list) {
          Serial.printf("%d ", data);
        }
        Serial.println();
      },
      1000);
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
MyClientCallbacks::MyClientCallbacks(int idx) { this->index = idx; };
void MyClientCallbacks::onConnect(BLEClient* pClient) {
  Serial.print(pClient->getPeerAddress().toString().c_str());
  Serial.println(" 加入连接");
  is_scan_down += 1;
}
void MyClientCallbacks::onDisconnect(BLEClient* pClient) {
  is_connected_device_list[this->index] = 0;  // 标志位清零
  Serial.print(pClient->getPeerAddress().toString().c_str());
  Serial.println(" 断开连接");
  is_scan_down -= 1;
}

// 连接设备
MyClient::MyClient() {
  this->BLEIndex = 0;
  this->pClient = nullptr;
  this->pService = nullptr;
  this->pNotifyChar = nullptr;
  this->pWriteChar = nullptr;
}
MyClient::MyClient(int idx) {
  this->BLEIndex = idx;
  this->pClient = nullptr;
  this->pService = nullptr;
  this->pNotifyChar = nullptr;
  this->pWriteChar = nullptr;
}
bool MyClient::write(uint8_t* data, size_t length) {
  if (this->pWriteChar == nullptr) {
    Serial.printf("pWriteChar 为空指针\n", this->BLEIndex);
    return false;
  }
  if (this->pWriteChar && this->pWriteChar->canWriteNoResponse()) {
    this->pWriteChar->writeValue(data, length);
    Serial.printf("设备-%d 写入数据成功\n", this->BLEIndex);
    return true;
  } else {
    Serial.printf("设备-%d 写入数据失败\n", this->BLEIndex);
    return false;
  }
}
bool MyClient::connect() {
  Serial.printf("设备-%d 正在连接...\n", this->BLEIndex);
  this->pClient = BLEDevice::createClient();
  this->pClient->setClientCallbacks(new MyClientCallbacks(this->BLEIndex));
  this->pClient->connect(pDevice);
  if (this->pClient->isConnected()) {
    Serial.printf("设备-%d 连接成功\n", this->BLEIndex);
  } else {
    Serial.printf("设备-%d 连接失败\n", this->BLEIndex);
    return false;
  }

  this->pService =
      this->pClient->getService("0000ffe5-0000-1000-8000-00805f9a34fb");
  if (this->pService == nullptr) {
    Serial.printf("设备-%d 获取服务失败\n", this->BLEIndex);
    return false;
  }
  Serial.printf("设备-%d 获取服务成功\n", this->BLEIndex);

  this->pNotifyChar =
      this->pService->getCharacteristic("0000FFE4-0000-1000-8000-00805F9A34FB");
  if (this->pNotifyChar && this->pNotifyChar->canNotify()) {
    Serial.printf("设备-%d 获取 Notify 特征成功\n", this->BLEIndex);
  } else {
    Serial.printf("设备-%d 获取 Notify 特征失败\n", this->BLEIndex);
    return false;
  }

  this->pWriteChar =
      this->pService->getCharacteristic("0000FFE9-0000-1000-8000-00805F9A34FB");
  // 此处该对象有值, 但是外部重新调用 pWriteChar 时为空指针
  // 原因: pWriteChar 为局部变量, 作用域仅限于此函数
  // 解决办法: 将 pWriteChar 作为类成员变量
  // 即: BLERemoteCharacteristic* pWriteChar;
  if (this->pWriteChar && this->pWriteChar->canWriteNoResponse()) {
    // pWriteCharList[this->BLEIndex] = this->pWriteChar;
    Serial.printf("设备-%d 获取 WriteNoRes 特征成功\n", this->BLEIndex);
  } else {
    Serial.printf("设备-%d 获取 WriteNoRes 特征失败\n", this->BLEIndex);
    return false;
  }

  auto idx = this->BLEIndex;

  auto myNotifyCallback = [idx](
                              BLERemoteCharacteristic* pBLERemoteCharacteristic,
                              uint8_t* pData, size_t length, bool isNotify) {
    // string str = string((char*)pData);
    // 示例数据: bytes: 0x55 0x61 0x5A 0x00 0xBE 0xFF 0x37 0xF8 0x00 0x00
    // 0x00 0x00 0x00 0x00 0x72 0x82 0x03 0xFD 0x19 0xB8
    // 数据包头: 0x55
    // 标志位: 0x61
    // 加速度X: 0x5A 0x00
    // 加速度Y: 0xBE 0xFF
    // 加速度Z: 0x37 0xF8
    // 角速度X: 0x00 0x00
    // 角速度Y: 0x00 0x00
    // 角速度Z: 0x00 0x00
    // 角度X: 0x72 0x82 -> Roll=((RollH<<8)+RollL)/32768*180;  //单位°
    // 角度Y: 0x03 0xFD -> Pitch=((PitchH<<8)+PitchL)/32768*180;  //单位°
    // 角度Z: 0x19 0xB8 -> Yaw=((YawH<<8)+YawL)/32768*180;  //单位°

    // 目标: 记录角度值
    // 找到包头 (不超过长度)
    // float Roll = 0.0, Pitch = 0.0, Yaw = 0.0;
    // int i = 0;
    // while (i < length - 1 && pData[i] != 0x55) {
    //   pData++;
    // }
    // // 找到标志位
    // if (pData[i + 1] == 0x61) {
    //   int16_t Roll = (pData[i + 12] << 8) + pData[i + 13];
    //   int16_t Pitch = (pData[i + 14] << 8) + pData[i + 15];
    //   int16_t Yaw = (pData[i + 16] << 8) + pData[i + 17];
    //   Serial.printf("设备-%d Roll: %d, Pitch: %d, Yaw: %d\n", idx,
    //   Roll,
    //                 Pitch, Yaw);
    // }
    // if (pData[0] != 0x55) {
    //   Serial.printf("设备-%d 数据包头错误\n", this->BLEIndex);
    //   return;
    // }

    device_data_list[idx]++;
    // if (idx == 0) {
    //   dcnt1++;
    // } else if (idx == 1) {
    //   dcnt2++;
    // } else if (idx == 2) {
    //   dcnt3++;
    // } else if (idx == 3) {
    //   dcnt4++;
    // } else if (idx == 4) {
    //   dcnt5++;
    // }
  };

  if (this->pNotifyChar->canNotify()) {
    this->pNotifyChar->registerForNotify(myNotifyCallback);
    Serial.printf("设备-%d 此特征值 支持 Notify\n", this->BLEIndex);
  } else {
    Serial.printf("设备-%d 此特征值 不支持 Notify\n", this->BLEIndex);
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Device started!");

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  bt_cfg.ble_max_act = 5;
  esp_bt_controller_init(&bt_cfg);
  esp_bt_controller_enable(ESP_BT_MODE_BLE);  // 启用BLE模式

  BLEDevice::init("ESP32_Bayyy");  // 初始化BLE设备, 其中的参数为设备名称
  pBLEScan = BLEDevice::getScan();  // 获取扫描对象
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdveertisedDeviceCallbacks(),
                                         true);  // 设置回调
  pBLEScan->setActiveScan(true);                 // 设置为主动扫描
  pBLEScan->setInterval(100);                    // 设置扫描间隔(ms)
  pBLEScan->setWindow(80);                       // 设置扫描窗口(ms)
  initDeviceAddressList();
  for (int i = 0; i < 5; i++) {
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
  } else if (str == "con") {
    if (doCon == 0) {
      Serial.println("识别到 con 指令");
      doCon = 1;
    }
  } else if (str.startsWith("rate")) {
    // 如果是: "ratex" -> 更改速率为 第x个
    if (str.length() < 5) {
      return;
    }
    int rate = str.substring(4).toInt();
    if (rate >= 0 && rate < changeRate.size() - 1) {
      uint8_t* data = changeRate[rate].data();
      Serial.printf("更改速率为: %d: ", rate);
      Serial.printf("写入: %x %x %x %x %x\n", data[0], data[1], data[2],
                    data[3], data[4]);
      // 遍历已连接设备, pWriteChar 写入数据
      for (int i = 0; i < client_list.size(); i++) {
        if (is_connected_device_list[i] == 1) {
          // Serial.printf("设备-%d 更改速率\n", i);
          client_list[i].write(data, 5);
          // if (pWriteCharList[i] != nullptr) {
          //   pWriteCharList[i]->writeValue(data, 5);
          //   Serial.printf("设备-%d 更改速率成功\n", i);
          // }
        }
      }
    }
  } else {
    Serial.println("Other");
  }
}