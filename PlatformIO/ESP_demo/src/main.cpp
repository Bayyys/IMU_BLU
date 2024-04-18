#include <main.h>

// 初始化设备地址列表 + 初始化定时器
void initDeviceAddressList() {
  for (auto address_str : device_address_str_list) {
    BLE_address_list.push_back(BLEAddress(address_str));
    device_connected_bytes.push_back(0x00);
  }
  t_name = t.setInterval(
      []() {
        if (!doPrint) {  // 如果不输出数据, 则输出连接状态
          Serial.write(device_connected_bytes.data(),
                       device_connected_bytes.size());
          Serial.println();
          return;
        }
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
        }
        // 数据输出
        uint8_t* data = dData.data();
        Serial.write(data, dData.size());
        Serial.println();
        device_data_update.assign(device_data_update.size(), 0);
      },
      100);
  for (int i = 0; i < device_address_str_list.size(); i++) {
    for (int j = 0; j < 6; j++) {
      dData.push_back(0x00);
    }
  }
}

void scan_handler() {
  string str = "开始扫描, 未连接设备列表: ";
  for (int i = 0; i < BLE_address_list.size(); i++) {
    if (is_connected_device_list[i] == 0) {
      str += to_string(i) + " ";
    }
  }
  str += ".";
  log_i("%s", str);
  if (doDebug) {
    Serial.println(str.c_str());
  }
  pBLEScan->clearResults();  // 清空搜索结果
  pBLEScan->start(5);        // 开始搜索
}

void con_handler() {
  log_i("连接至设备-%d\n", pConIndex);
  MyClient client = client_list[pConIndex];
  if (client.connect()) {
    log_i("设备-%d Connect successfully!\n", pConIndex);
  } else {
    log_e("设备-%d Connect failed!\n", pConIndex);
  }
  doCon = 0;
  doScan = 1;
}

// 扫描设备回调
void MyAdveertisedDeviceCallbacks::onResult(
    BLEAdvertisedDevice advertisedDevice) {
  // 判断名字是否以 "WT901" 开头
  if (advertisedDevice.getName().find("WT901") != std::string::npos) {
    log_i("[MyAdveertisedDeviceCallbacks] 扫描到设备: %s\n",
          advertisedDevice.getAddress().toString().c_str());
    // 判断是否在连接列表中
    for (int i = 0; i < BLE_address_list.size(); i++) {
      // 扫描到设备且未连接
      if (advertisedDevice.getAddress() == BLE_address_list[i] &&
          is_connected_device_list[i] == 0) {
        log_i("[MyAdveertisedDeviceCallbacks] 扫描可连接设备-%d: %s\n", i,
              advertisedDevice.getAddress().toString().c_str());
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
  log_i("%s 加入连接.", pClient->getPeerAddress().toString().c_str());
  is_scan_down += 1;
  is_connected_device_list[client_index_] = 1;
  device_connected_bytes[client_index_ + 2] = 0x01;
  // 输出连接信息
  Serial.write(device_connected_bytes.data(), device_connected_bytes.size());
  Serial.println();
  if (is_scan_down == BLE_address_list.size()) {
    log_i("-------所有设备已连接---------");
    doScan = 0;
    if (!doPrint) {
      log_i("-------开始输出数据---------");
    }
  }
}
void MyClientCallbacks::onDisconnect(BLEClient* pClient) {
  is_connected_device_list[client_index_] = 0;  // 标志位清零
  device_connected_bytes[client_index_ + 2] = 0x00;
  log_e("%s 断开连接.", pClient->getPeerAddress().toString().c_str());
  is_scan_down -= 1;
  doPrint = 0;
  doScan = 1;
  log_e("----- 设备-%d 断开连接 -----\n", client_index_);
  log_e("----- 剩余设备数: %d ----\n", is_scan_down);
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
  log_i("设备-%d 正在连接...\n", BLEIndex_);
  pClient_->setClientCallbacks(new MyClientCallbacks(BLEIndex_));
  pClient_->connect(pDevice);
  if (pClient_->isConnected()) {
    log_i("设备-%d 连接成功\n", BLEIndex_);
  } else {
    log_e("设备-%d 连接失败\n", BLEIndex_);
    return false;
  }

  pService_ = pClient_->getService("0000ffe5-0000-1000-8000-00805f9a34fb");
  if (pService_ == nullptr) {
    log_e("设备-%d 获取服务失败\n", BLEIndex_);
    return false;
  }
  log_i("设备-%d 获取服务成功\n", BLEIndex_);

  pNotifyChar_ =
      pService_->getCharacteristic("0000FFE4-0000-1000-8000-00805F9A34FB");
  if (pNotifyChar_ && pNotifyChar_->canNotify()) {
    log_i("设备-%d 获取 Notify 特征成功\n", BLEIndex_);
  } else {
    log_e("设备-%d 获取 Notify 特征失败\n", BLEIndex_);
    return false;
  }

  pWriteChar_ =
      pService_->getCharacteristic("0000FFE9-0000-1000-8000-00805F9A34FB");
  if (pWriteChar_ && pWriteChar_->canWriteNoResponse()) {
    pWriteCharList[BLEIndex_] = pWriteChar_;
    log_i("设备-%d 获取 WriteNoRes 特征成功\n", BLEIndex_);
    pWriteChar_->writeValue(changeRate[initRate].data(), 5);  // 默认速率为 10Hz
  } else {
    log_e("设备-%d 获取 WriteNoRes 特征失败\n", BLEIndex_);
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
    log_i("设备-%d 此特征值 支持 Notify\n", BLEIndex_);
  } else {
    log_e("设备-%d 此特征值 不支持 Notify\n", BLEIndex_);
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  log_i("Device started!");

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
  log_i("收到指令 %s", str);
  // 去除换行符、回车及空格
  str.trim();
  if (str == "stop") {
    doScan = 0;
  } else if (str == "scan") {
    doScan = 1;
  } else if (str == "printy") {
    if (is_scan_down >= BLE_address_list.size()) {
      doPrint = 1;
    } else {
      if (doDebug) log_e("未连接所有设备, 无法输出数据");
    }
  } else if (str == "printn") {
    doPrint = 0;
  } else if (str.startsWith("rate")) {
    // 如果是: "ratex" -> 更改速率为 第x个
    if (str.length() < 5) {
      return;
    }
    int rate = str.substring(4).toInt();
    if (rate >= 0 && rate < changeRate.size() - 1) {
      uint8_t* data = changeRate[rate].data();
      log_i("更改速率为: %.1f Hz\n", rateHz[rate]);
      t.changeDelay(t_name, timerMs[rate]);  // 更改定时器时间
      log_i("写入: %x %x %x %x %x\n", data[0], data[1], data[2], data[3],
            data[4]);
      // 遍历已连接设备, pWriteChar 写入数据
      for (int i = 0; i < client_list.size(); i++) {
        if (is_connected_device_list[i] == 1) {
          if (pWriteCharList[i] != nullptr) {
            pWriteCharList[i]->writeValue(data, 5);
            log_i("设备-%d 更改速率成功\n", i);
          }
        }
      }
    }
  } else if (str == "debugy") {
    doDebug = doDebug == 0 ? 1 : 0;
    log_i("调试模式: 开启");
  } else if (str == "debugn") {
    doDebug = 0;
    log_i("调试模式: 关闭");
  } else {
    log_e("Other");
  }
}