package com.example.bletestutils.utils;

import android.app.Application;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.util.Log;

import com.clj.fastble.BleManager;
import com.clj.fastble.callback.BleGattCallback;
import com.clj.fastble.callback.BleMtuChangedCallback;
import com.clj.fastble.callback.BleNotifyCallback;
import com.clj.fastble.callback.BleReadCallback;
import com.clj.fastble.callback.BleRssiCallback;
import com.clj.fastble.callback.BleScanCallback;
import com.clj.fastble.callback.BleWriteCallback;
import com.clj.fastble.data.BleDevice;
import com.clj.fastble.scan.BleScanRuleConfig;
import com.example.bletestutils.BaseActivity;


import java.util.HashMap;
import java.util.List;

public class FastBleUtils {

    private static FastBleUtils myFastBleUtils;
    private String TAG=getClass().getSimpleName();
    public static FastBleUtils getInstant(){
        synchronized (FastBleUtils.class){
            if (myFastBleUtils==null)myFastBleUtils=new FastBleUtils();
            return myFastBleUtils;
        }

    }
    public void init(Application app){
        BleManager.getInstance().init(app);
        BleManager.getInstance()
                .enableLog(true)
                .setReConnectCount(1, 5000)
                .setOperateTimeout(5000);
    }

    //扫描之前可以设置过滤你要过滤的蓝牙
    public void scanSetting(){
        //        BleScanRuleConfig scanRuleConfig = new BleScanRuleConfig.Builder()
//                .setServiceUuids(serviceUuids)      // 只扫描指定的服务的设备，可选
//                .setDeviceName(true, names)         // 只扫描指定广播名的设备，可选
//                .setDeviceMac(mac)                  // 只扫描指定mac的设备，可选
//                .setAutoConnect(isAutoConnect)      // 连接时的autoConnect参数，可选，默认false
//                .setScanTimeOut(10000)              // 扫描超时时间，可选，默认10秒
//                .build();
        BleScanRuleConfig scanRuleConfig = new BleScanRuleConfig.Builder()
                .setAutoConnect(false) // 连接时的autoConnect参数，可选，默认false   先不要自动连接，避免连接失败自动重连时间过程，连接失败手动马上断开重连，提高效率
                .setScanTimeOut(-1)
                .build();
        BleManager.getInstance().initScanRule(scanRuleConfig);
    }
    public void scanBle(BleScanCallback bleScanCallback){
        //先设置扫描的参数
        scanSetting();
        BleManager.getInstance().scan(bleScanCallback);
    }
    public void cancelScan(){
            BleManager.getInstance().cancelScan();
    }
    public void connectBle(BleDevice bleDevice,BleGattCallback bleGattCallback){
        BleManager.getInstance().connect(bleDevice, bleGattCallback);
    }
    //断开链接
    public void bleDisConnect(BleDevice bleDevice){
        BleManager.getInstance().disconnect(bleDevice);
    }

    /**
     * 接收通知
     *
     * 有两种方式可以接收通知，indicate和notify。indicate和notify的区别就在于，indicate是一定会收到数据，
     * notify有可能会丢失数据。indicate底层封装了应答机制，如果没有收到中央设备的回应，会再次发送直至成功；而notify不会有central收到数据的回应，
     * 可能无法保证数据到达的准确性，优势是速度快。通常情况下，当外围设备需要不断地发送数据给APP的时候，比如血压计在测量过程中的压力变化，胎心仪在监护过程中的实时数据传输，
     * 这种频繁的情况下，优先考虑notify形式。当只需要发送很少且很重要的一条数据给APP的时候，优先考虑indicate形式。当然，从Android开发角度的出发，如果硬件放已经考虑了成熟的协议和发送方式，
     * 我们需要做的仅仅是根据其配置的数据发送方式进行相应的对接即可。
     */
    /**
     *     new BleNotifyCallback() {
     *                     @Override
     *                     public void onNotifySuccess() {
     *                         // 打开通知操作成功
     *                     }
     *
     *                     @Override
     *                     public void onNotifyFailure(BleException exception) {
     *                         // 打开通知操作失败
     *                     }
     *
     *                     @Override
     *                     public void onCharacteristicChanged(byte[] data) {
     *                         // 打开通知后，设备发过来的数据将在这里出现
     *                     }
     *                 }
     * @param bleDevice
     * @param uuid_service
     * @param uuid_characteristic_notify
     * @param bleNotifyCallback
     */
    public void openNotify(BleDevice bleDevice, String uuid_service, String uuid_characteristic_notify, BleNotifyCallback bleNotifyCallback){
        BleManager.getInstance().notify(
                bleDevice,
                uuid_service,
                uuid_characteristic_notify,bleNotifyCallback
            );
    }
    public void closeNotify(BleDevice device,String uuid_service,String uuid_characteristic_notify){
        BleManager.getInstance().stopNotify(device,uuid_service, uuid_characteristic_notify);
    }
// new BleReadCallback() {
//                    @Override
//                    public void onReadSuccess(byte[] data) {
//                        // 读特征值数据成功
//                    }
//
//                    @Override
//                    public void onReadFailure(BleException exception) {
//                        // 读特征值数据失败
//                    }
//                }
    public void readBleData(BleDevice bleDevice,String uuid_service,String uuid_characteristic_read,BleReadCallback bleReadCallback){
        BleManager.getInstance().read(
                bleDevice,
                uuid_service,
                uuid_characteristic_read,
                bleReadCallback);
    }
    //new BleWriteCallback() {
    //                    @Override
    //                    public void onWriteSuccess(int current, int total, byte[] justWrite) {
    //                        // 发送数据到设备成功（分包发送的情况下，可以通过方法中返回的参数可以查看发送进度）
    //                    }
    //
    //                    @Override
    //                    public void onWriteFailure(BleException exception) {
    //                        // 发送数据到设备失败
    //                    }
    //                }
    public void writeBleData(BleDevice bleDevice, String uuid_service, String uuid_characteristic_write, byte[]data, BleWriteCallback bleWriteCallback){
        BleManager.getInstance().write(
                bleDevice,
                uuid_service,
                uuid_characteristic_write,
                data,
                bleWriteCallback);
    }
    public void writeBleData(BaseActivity baseActivity, BleDevice bleDevice , String mapKey, byte[]data, BleWriteCallback bleWriteCallback){
        BluetoothGattCharacteristic characteristic = characteristicMap.get(mapKey);
        if (characteristic==null){
            baseActivity.toastText("没找到特征！");
            return;
        }
        BleManager.getInstance().write(
                bleDevice,
                serviceF.getUuid().toString(),
                characteristic.getUuid().toString(),
                data,
                bleWriteCallback);
    }
    public void writeName(BaseActivity activity,BleDevice bleDevice,byte[] data,BleWriteCallback bleWriteCallback){
        writeBleData(activity,bleDevice,NAME,data,bleWriteCallback);
    }
    public void writeTime(BaseActivity activity,BleDevice bleDevice,String progress,BleWriteCallback bleWriteCallback){
//        writeBleData(activity,bleDevice, TIME, HexUtil.hexStringToBytes(progress),bleWriteCallback);
    }

    /**new BleMtuChangedCallback() {
    @Override
    public void onSetMTUFailure(BleException exception) {
    // 设置MTU失败
    }

    @Override
    public void onMtuChanged(int mtu) {
    // 设置MTU成功，并获得当前设备传输支持的MTU值
    }
    }
     * 进行BLE数据相互发送的时候，一次最多能发送20个字节。
     * 如果需要发送的数据超过20个字节，有两种方法，一种是主动尝试拓宽MTU，另一种是采用分包传输的方式。框架中的write方法，
     * 当遇到数据超过20字节的情况时，默认是进行分包发送的。
     */
    public void setMtu(BleDevice bleDevice, int mtu, BleMtuChangedCallback bleMtuChangedCallback){
        //设置最大传输单元MTU
        BleManager.getInstance().setMtu(bleDevice, mtu, bleMtuChangedCallback);
    }

    /**
     *  new BleRssiCallback() {
     *
     *                     @Override
     *                     public void onRssiFailure(BleException exception) {
     *                         // 读取设备的信号强度失败
     *                     }
     *
     *                     @Override
     *                     public void onRssiSuccess(int rssi) {
     *                         // 读取设备的信号强度成功
     *                     }
     *                 }
     * @param bleDevice
     * @param bleRssiCallback
     */
    public void getBleDevRssi(BleDevice bleDevice, BleRssiCallback bleRssiCallback){
        //获取设备的实时信号强度Rssi
        BleManager.getInstance().readRssi(
                bleDevice,bleRssiCallback);
    }

    public void recordCharacteristic(BluetoothGatt gatt){
        recordCharacteristic(gatt,null);
    }
    //服务特征可能有很多个，不同产品不一样。我们公司有一定的标识文档，包含某些字样的就是不同信息的特征
    public void recordCharacteristic(BluetoothGatt gatt,PWDCallBack pwdCallBack){
        serviceF=null;
        characteristicMap.clear();
        List<BluetoothGattService> serviceList = gatt.getServices();
        for (BluetoothGattService service : serviceList) {
            if (service.getUuid().toString().contains("ff00")) {//如果有多个服务，常规来说都可以发，但我们公司产品指定包含ff00才是我们的服务
                serviceF = service;

            }
            List<BluetoothGattCharacteristic> characteristicList= service.getCharacteristics();
            for(BluetoothGattCharacteristic characteristic : characteristicList) {
                if (pwdCallBack!=null)pwdCallBack.logeMesg("服务特征:"+characteristic.getUuid());
                Log.e("MyFastBleUtils", "recordCharacteristic: UUID"+characteristic.getUuid());
                byte[] value1 = characteristic.getValue();
                Log.e(TAG, "recordCharacteristic: "+value1 );
                if (value1!=null) Log.e(TAG, "recordCharacteristic: "+new String(value1) );
                if (characteristic.getUuid().toString().contains(CompanyID)){
                    characteristicMap.put(CompanyID,characteristic);
                } else if (characteristic.getUuid().toString().contains(UUID)) {
                    characteristicMap.put(UUID,characteristic);
                }else if (characteristic.getUuid().toString().contains(major)){
                    characteristicMap.put(major,characteristic);
                }else if (characteristic.getUuid().toString().contains(minor)){
                    characteristicMap.put(minor,characteristic);
                }else if (characteristic.getUuid().toString().contains(RSSI)){
                    characteristicMap.put(RSSI,characteristic);
                }else if (characteristic.getUuid().toString().contains(VOLTAGE)){
                    characteristicMap.put(VOLTAGE,characteristic);
                }else if (characteristic.getUuid().toString().contains(BROADCAST)){
                    characteristicMap.put(BROADCAST,characteristic);
                }else if (characteristic.getUuid().toString().contains(POWER)){
                    characteristicMap.put(POWER,characteristic);
                }else if (characteristic.getUuid().toString().contains(NAME)){
                    characteristicMap.put(NAME,characteristic);
                }else if (characteristic.getUuid().toString().contains(PWD)){
                    characteristicMap.put(PWD,characteristic);
                    //这个是我们公司的密码特征，里边记录了密码，如果没有，我们公司默认是null
                    byte[] value = characteristic.getValue();
                    if (pwdCallBack!=null&&value!=null){
                        pwdCallBack.hasPwd(value);
                    }
                    if (value!=null)
                    Log.e(TAG, "recordCharacteristic:特征密码="+new String(value));
                    else Log.e(TAG, "recordCharacteristic:特征的 byte 密码= "+value );
                }
            }
        }
    }
    //公司的服务特征，
    public BluetoothGattService serviceF;
    final HashMap<String,BluetoothGattCharacteristic>characteristicMap=new HashMap<>();
    public static String CompanyID= "ff01";
    public static String UUID= "ff02";
    public static String major= "ff03";
    public static String minor= "ff04";
    public static String RSSI= "ff05";
    public static String VOLTAGE= "ff06";//电压
    public static String BROADCAST = "ff07";//广播间隔
    public static String POWER = "ff08";//发送功率
    public static String NAME = "ff09";//广播名称
    public static String PWD = "ff0a";//密码


    public HashMap<String, BluetoothGattCharacteristic> getCharacteristicMap() {
        return characteristicMap;
    }

    public interface PWDCallBack{
        public void hasPwd(byte[] value);
        public void logeMesg(String log);
    }
}
