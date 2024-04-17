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

AsyncTimer t;                             // 定时器
BLEScan* pBLEScan;                        // 扫描对象
int doScan = 0;                           // 是否扫描
int doCon = 0;                            // 是否连接
vector<BLEAdvertisedDevice> device_list;  // 搜索到的设备列表
BLEAdvertisedDevice* pDevice;             // 搜索到等待连接的设备
int pConIndex = -1;                       // 待连接设备的编号

int dcnt1 = 0;
int dcnt2 = 0;
int dcnt3 = 0;
int dcnt4 = 0;
int dcnt5 = 0;
int device_data_list[5] = {0, 0, 0, 0, 0};          // 设备数据
int is_connected_device_list[5] = {0, 0, 0, 0, 0};  // 是否连接设备
int is_scan_down = 0;                               // 是否扫描结束
// 存储所有设备的写服务特征值
BLERemoteCharacteristic* pWriteCharList[5] = {nullptr, nullptr, nullptr,
                                              nullptr, nullptr};
// 更改速率特征值数据
vector<vector<uint8_t>> changeRate = {
    {0xFF, 0xAA, 0x03, 0x01, 0x00},  // 0.1Hz
    {0xFF, 0xAA, 0x03, 0x02, 0x00},  // 0.5Hz
    {0xFF, 0xAA, 0x03, 0x03, 0x00},  // 1Hz
    {0xFF, 0xAA, 0x03, 0x04, 0x00},  // 2Hz
    {0xFF, 0xAA, 0x03, 0x05, 0x00},  // 5Hz
    {0xFF, 0xAA, 0x03, 0x06, 0x00},  // 10Hz
    {0xFF, 0xAA, 0x03, 0x07, 0x00},  // 20Hz
    {0xFF, 0xAA, 0x03, 0x08, 0x00},  // 50Hz
    {0xFF, 0xAA, 0x03, 0x09, 0x00},  // 100Hz
    {0xFF, 0xAA, 0x03, 0x0A, 0x00},  // 200Hz
};

void connectToDevices(BLEAdvertisedDevice device, int index);

void initDeviceAddressList();

void scan_handler();

void con_handler();

class MyAdveertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice);
};

class MyClientCallbacks : public BLEClientCallbacks {
 private:
  int index{0};

 public:
  MyClientCallbacks(int idx);
  void onConnect(BLEClient* pClient);
  void onDisconnect(BLEClient* pClient);
};

class MyClient {
 private:
  int BLEIndex = {0};
  BLEClient* pClient;
  BLERemoteService* pService;
  BLERemoteCharacteristic* pNotifyChar;
  BLERemoteCharacteristic* pWriteChar;

 public:
  MyClient();
  MyClient(int index);
  bool write(uint8_t* data, size_t length);
  bool connect();
};

// 设备连接列表
vector<MyClient> client_list;