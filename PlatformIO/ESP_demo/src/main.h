#include <Arduino.h>
#include <AsyncTimer.h>
#include <BLEDevice.h>

#include <vector>
using namespace std;

vector<BLEAddress> BLE_address_list;  // 需要连接的设备地址列表
vector<string> device_address_str_list = {
    "CF:88:10:CB:0A:2E",  // 设备 0
    "F3:8C:9A:CF:F0:D7",  // 设备 1
    "D9:69:71:72:2D:1E",  // 设备 2
    "FF:E6:12:3B:08:A6",  // 设备 3
                          // "E5:4B:A7:C7:85:32",  // 设备 4
};

AsyncTimer t;              // 定时器
unsigned long t_name = 0;  // 定时器名称

BLEScan* pBLEScan;                        // 扫描对象
int doScan = 1;                           // 是否扫描
int doCon = 0;                            // 是否连接
int doPrint = 0;                          // 是否输出数据量
int doDebug = 0;                          // 调试模式, 输出调试信息
vector<BLEAdvertisedDevice> device_list;  // 搜索到的设备列表
BLEAdvertisedDevice* pDevice;             // 搜索到等待连接的设备
int pConIndex = -1;                       // 待连接设备的编号

int device_data_list[4] = {0, 0, 0, 0};          // 设备数据
int is_connected_device_list[4] = {0, 0, 0, 0};  // 是否连接设备
vector<uint8_t> device_connected_bytes = {0x55, 0x62};
vector<int> device_data_update = {0, 0, 0, 0};  // 设备数据更新标志
vector<uint8_t> dData = {0x55, 0x61};
vector<vector<float>> device_data = {
    {0, 0, 0},  // 设备 0
    {0, 0, 0},  // 设备 1
    {0, 0, 0},  // 设备 2
    {0, 0, 0},  // 设备 3
};
int is_scan_down = 0;  // 是否扫描结束(记录连接设备数)
// 存储所有设备的写服务特征值
BLERemoteCharacteristic* pWriteCharList[4] = {nullptr, nullptr, nullptr,
                                              nullptr};
// 更改速率特征值数据
vector<vector<uint8_t>> changeRate = {
    {0xFF, 0xAA, 0x03, 0x01, 0x00},  // 0: 0.1Hz
    {0xFF, 0xAA, 0x03, 0x02, 0x00},  // 1: 0.5Hz
    {0xFF, 0xAA, 0x03, 0x03, 0x00},  // 2: 1Hz
    {0xFF, 0xAA, 0x03, 0x04, 0x00},  // 3: 2Hz
    {0xFF, 0xAA, 0x03, 0x05, 0x00},  // 4: 5Hz
    {0xFF, 0xAA, 0x03, 0x06, 0x00},  // 5: 10Hz
    {0xFF, 0xAA, 0x03, 0x07, 0x00},  // 6: 20Hz
    {0xFF, 0xAA, 0x03, 0x08, 0x00},  // 7: 50Hz
    {0xFF, 0xAA, 0x03, 0x09, 0x00},  // 8: 100Hz
    {0xFF, 0xAA, 0x03, 0x0A, 0x00},  // 9: 200Hz
};
vector<float> rateHz = {0.1, 0.5, 1, 2, 5, 10, 20, 50, 100, 200};
vector<int> timerMs = {10000, 2000, 1000, 500, 200, 100, 50, 20, 10, 5};
int initRate = 2;  // 初始化速率

void connectToDevices(BLEAdvertisedDevice device, int index);

void initDeviceAddressList();

void scan_handler();

void con_handler();

class MyAdveertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice);
};

class MyClientCallbacks : public BLEClientCallbacks {
 private:
  int client_index_{0};

 public:
  MyClientCallbacks(int idx);
  void onConnect(BLEClient* pClient);
  void onDisconnect(BLEClient* pClient);
};

class MyClient {
 private:
  int BLEIndex_ = {0};
  BLEClient* pClient_;
  BLERemoteService* pService_;
  BLERemoteCharacteristic* pNotifyChar_;
  BLERemoteCharacteristic* pWriteChar_;

 public:
  MyClient();
  MyClient(int index);
  bool connect();
};

// 设备连接列表
vector<MyClient> client_list;