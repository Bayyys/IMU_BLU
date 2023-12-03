package com.example.bletestutils;


import android.annotation.SuppressLint;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.ScrollView;

import com.clj.fastble.callback.BleGattCallback;
import com.clj.fastble.callback.BleMtuChangedCallback;
import com.clj.fastble.callback.BleNotifyCallback;
import com.clj.fastble.callback.BleWriteCallback;
import com.clj.fastble.data.BleDevice;
import com.clj.fastble.exception.BleException;
import com.clj.fastble.utils.HexUtil;
import com.example.bletestutils.databinding.ActivityControlBinding;
import com.example.bletestutils.dialogs.ConfirmDialog;
import com.example.bletestutils.dialogs.TextListDialog;
import com.example.bletestutils.utils.FastBleUtils;
import com.google.gson.Gson;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;
import java.util.function.Consumer;

@SuppressLint("MissingPermission")
public class ControlActivity extends BaseActivity implements View.OnClickListener {
    private com.example.bletestutils.databinding.ActivityControlBinding inflate;
    private BleDevice dev;
    private BleDevice currentBleDev;

    private UUID SERVICE_UUUID = UUID.fromString("0000fff0-0000-1000-8000-00805f9b34fb");
    private UUID SERVICEF000_UUUID = UUID.fromString("0000f000-0000-1000-8000-00805f9b34fb");
    private UUID WRITE_UUUID = UUID.fromString("0000fff2-0000-1000-8000-00805f9b34fb");
    private UUID NOTIFY_UUUID = UUID.fromString("0000fff1-0000-1000-8000-00805f9b34fb");
    private UUID F001_UUUID = UUID.fromString("0000f001-0000-1000-8000-00805f9b34fb");
    private UUID F002_UUUID = UUID.fromString("0000f002-0000-1000-8000-00805f9b34fb");


    private long time;
    long sendSize = 0;
    long receviceSize = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        inflate = ActivityControlBinding.inflate(getLayoutInflater());
        setContentView(inflate.getRoot());
        getIntentData();
        inflate.btnSetMtu.setOnClickListener(this);
        inflate.btnSelectNotify.setOnClickListener(this);
        inflate.btnSelectWrite.setOnClickListener(this);
        inflate.btnSend.setOnClickListener(this);
        inflate.btnClean.setOnClickListener(this);

        inflate.btnF001H.setOnClickListener(this);
        inflate.btnF001L.setOnClickListener(this);
        inflate.btnF002H.setOnClickListener(this);
        inflate.btnF002L.setOnClickListener(this);
        inflate.cbAutoSend.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                autoSend(isChecked);
            }
        });
        inflate.etMtu.setText("100");
        inflate.etTime.setText("100");
        inflate.etMtu.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                String value = s.toString();
                if (!TextUtils.isEmpty(value)) {
                    double v = Double.parseDouble(value);
                    if (v > 512) {
                        inflate.etMtu.setText("512");
                        inflate.etMtu.setSelection("512".length());
                    }
                }

            }

            @Override
            public void afterTextChanged(Editable s) {

            }
        });

    }


    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (currentBleDev != null) {
            FastBleUtils.getInstant().bleDisConnect(currentBleDev);
        }
        hSend.removeCallbacksAndMessages(null);
        hmtu.removeCallbacksAndMessages(null);
        hConnect.removeCallbacksAndMessages(null);
    }

    private void autoSend(boolean isChecked) {
        hSend.removeCallbacksAndMessages(null);
        if (isChecked) {
            String s = inflate.etTime.getText().toString();
            try {
                time = (long) Double.parseDouble(s);
            } catch (NumberFormatException e) {
                Log.e(TAG, "autoSend: " + e);
            }
            hSend.postDelayed(rsend, time);
        }
    }

    Handler hSend = new Handler();
    Runnable rsend = new Runnable() {
        @Override
        public void run() {
            boolean checked = inflate.cbSendCheckbox.isChecked();
            byte[] bytes;
            if (checked) {
                bytes = HexUtil.hexStringToBytes(inflate.etContent.getText().toString());
                Log.e(TAG, "run:toJson1: " + new Gson().toJson(bytes));
            } else {
                bytes = inflate.etContent.getText().toString().getBytes();
                Log.e(TAG, "run:toJson2: " + new Gson().toJson(bytes));
            }
            writeData(bytes);
            String s = inflate.etTime.getText().toString();
            try {
                time = (long) Double.parseDouble(s);
            } catch (NumberFormatException e) {
                Log.e(TAG, "autoSend: " + e);
            }
            hSend.postDelayed(rsend, time);
        }
    };

    private void getIntentData() {
        Intent intent = getIntent();
        dev = intent.getParcelableExtra("dev");
        if (dev == null) {
            toastText("无法获取蓝牙参数");
            finish();
        }
        bleConnect(dev, 0);

    }

    /**
     * 设置MTU，需要在设备连接之后进行操作。
     * 默认每一个BLE设备都必须支持的MTU为23。
     * 在Android 低版本(API-17 到 API-20)上，没有这个限制。所以只有在API21以上的设备，才会有拓展MTU这个需求。
     * 该方法的参数mtu，最小设置为23，最大设置为512。
     * 并不是每台设备都支持拓展MTU，需要通讯双方都支持才行，也就是说，需要设备硬件也支持拓展MTU该方法才会起效果。调用该方法后，可以通过onMtuChanged(int mtu)查看最终设置完后，设备的最大传输单元被拓展到多少。如果设备不支持，可能无论设置多少，最终的mtu还是23。
     */
    Handler hmtu = new Handler();
    int mtu = 100;

    private void setmtu(long delay) {
        try {
            mtu = (int) Double.parseDouble(inflate.etMtu.getText().toString());
        } catch (NumberFormatException e) {

        }
        hmtu.removeCallbacksAndMessages(null);
        hmtu.postDelayed(new Runnable() {
            @Override
            public void run() {
                FastBleUtils.getInstant().setMtu(currentBleDev, mtu, new BleMtuChangedCallback() {

                    @Override
                    public void onSetMTUFailure(BleException exception) {
//                        boolean b = getThis().hasWindowFocus();

//                        if (b)
//                      new ConfirmDialog(getThis()) {
//                          @Override
//                          public void btnSure() {
//
//                          }
//
//                          @Override
//                          protected void btnCancel() {
//
//                          }
//                      }.setTips("设置mtu失败：mtu="+mtu+";exception:"+exception.toString()).show();

                    }

                    @Override
                    public void onMtuChanged(int mtu) {
                        new ConfirmDialog(getThis()) {
                            @Override
                            public void btnSure() {

                            }

                            @Override
                            protected void btnCancel() {

                            }
                        }.setTips("设置mtu成功,mtu=" + mtu).show();
                    }
                });
            }
        }, delay);

    }

    Handler hConnect = new Handler();

    private void bleConnect(BleDevice bleDevice, long delay) {

        Log.e(TAG, "bleConnect: " + (bleDevice == null));
        hConnect.removeCallbacksAndMessages(null);
        hConnect.postDelayed(new Runnable() {
            @Override
            public void run() {
                //说明是我们的产品，风扇具体看实际
                FastBleUtils.getInstant().connectBle(bleDevice, new BleGattCallback() {
                    @Override
                    public void onStartConnect() {
                        Log.e(TAG, "onStartConnect: ");
                    }

                    @Override
                    public void onConnectFail(BleDevice bleDevice, BleException exception) {
                        Log.e(TAG, "onConnectFail: " + exception);
                        toastText("链接失败");

                    }

                    @SuppressLint("MissingPermission")
                    @Override
                    public void onConnectSuccess(BleDevice bleDevice, BluetoothGatt gatt, int status) {
                        Log.e(TAG, "onConnectSuccess:");
                        if (status != BluetoothGatt.GATT_SUCCESS) {
                            Log.e(TAG, "onConnectSuccess:bleDis Connect ");
                            FastBleUtils.getInstant().bleDisConnect(bleDevice);
                            return;
                        }
                        currentBleDev = bleDevice;
                        List<BluetoothGattService> services = gatt.getServices();
                        if (services.isEmpty()) {
                            Log.e(TAG, "onConnectSuccess: 服务特征为空！");
                        } else {
                            services.forEach(new Consumer<BluetoothGattService>() {
                                @Override
                                public void accept(BluetoothGattService bluetoothGattService) {

                                    List<BluetoothGattCharacteristic> characteristics = bluetoothGattService.getCharacteristics();
                                    Log.e(TAG, "accept:ServiceUUID: " + bluetoothGattService.getUuid().toString());
                                    if (characteristics != null)
                                        characteristics.forEach(new Consumer<BluetoothGattCharacteristic>() {
                                            @Override
                                            public void accept(BluetoothGattCharacteristic bluetoothGattCharacteristic) {
                                                Log.e(TAG, "accept:getUuid " + bluetoothGattCharacteristic.getUuid());
                                            }
                                        });
                                }
                            });
                        }
                        toastText("连接成功");

                        openNotify(bleDevice, 100);
                        setmtu(200);
                    }

                    @Override
                    public void onDisConnected(boolean isActiveDisConnected, BleDevice device, BluetoothGatt gatt, int status) {
                        try {
                            toastText("断开连接。");

                            Log.e(TAG, "onDisConnected: ");
                        } catch (Exception e) {
                            Log.e(TAG, "onDisConnected: " + e);
                        }
                    }
                });
            }
        }, delay);
    }

    Handler nH = new Handler();

    private void openNotify(BleDevice bleDevice, long delay) {
        nH.removeCallbacksAndMessages(null);
        Log.e(TAG, "openNotify: 开启通知:" + NOTIFY_UUUID.toString());
        nH.postDelayed(new Runnable() {
            @Override
            public void run() {
                FastBleUtils.getInstant().openNotify(bleDevice, SERVICE_UUUID.toString(), NOTIFY_UUUID.toString(), new BleNotifyCallback() {
                    @Override
                    public void onNotifySuccess() {
                        logtv("onNotifySuccess: ");
                    }

                    @Override
                    public void onNotifyFailure(BleException exception) {
                        logtv("onNotifyFailure: ");
                    }

                    @Override
                    public void onCharacteristicChanged(byte[] data) {
                        logtv((inflate.cbCheckbox.isChecked() ? HexUtil.formatHexString(data) : new String(data)));
                        if (data != null)
                            receviceSize += data.length;
                        showSize();
                    }
                });
            }
        }, delay);
    }

    BleWriteCallback bleWriteCallback = new BleWriteCallback() {
        @Override
        public void onWriteSuccess(int current, int total, byte[] justWrite) {
            Log.e(TAG, "onWriteSuccess: " + new String(justWrite) + ";hexStr:" + HexUtil.formatHexString(justWrite));
//            logtv("Write:"+(inflate.cbCheckbox.isChecked()?HexUtil.formatHexString(justWrite):new Gson().toJson(justWrite)));
            if (justWrite != null)
                sendSize += justWrite.length;
            showSize();
        }

        @Override
        public void onWriteFailure(BleException exception) {
            toastText("onWriteFailure");
            Log.e(TAG, "onWriteFailure: " + exception);
        }
    };
    BleWriteCallback bleWriteCallbackf001_f002 = new BleWriteCallback() {
        @Override
        public void onWriteSuccess(int current, int total, byte[] justWrite) {
            Log.e(TAG, "onWriteSuccess: " + new String(justWrite) + ";hexStr:" + HexUtil.formatHexString(justWrite));
            if (justWrite != null)
                sendSize += justWrite.length;
            showSize();
//            logtv("Write:"+(inflate.cbCheckbox.isChecked()?HexUtil.formatHexString(justWrite):new Gson().toJson(justWrite)));
        }

        @Override
        public void onWriteFailure(BleException exception) {
            Log.e(TAG, "onWriteFailure: " + exception);
//            toastText("onWriteFailure");
        }
    };

    public void writeData(byte[] data) {
        FastBleUtils.getInstant().writeBleData(currentBleDev, SERVICE_UUUID.toString(), WRITE_UUUID.toString(), data, bleWriteCallback);
    }

    public void writeDataF001(byte[] data) {
        FastBleUtils.getInstant().writeBleData(currentBleDev, SERVICEF000_UUUID.toString(), F001_UUUID.toString(), data, bleWriteCallbackf001_f002);
    }

    public void writeDataF002(byte[] data) {
        FastBleUtils.getInstant().writeBleData(currentBleDev, SERVICEF000_UUUID.toString(), F002_UUUID.toString(), data, bleWriteCallbackf001_f002);
    }

    byte[] height = HexUtil.hexStringToBytes("01");
    byte[] lower = HexUtil.hexStringToBytes("00");

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_send:
                boolean checked = inflate.cbSendCheckbox.isChecked();
                byte[] bytes;
                if (checked) {
                    bytes = HexUtil.hexStringToBytes(inflate.etContent.getText().toString());
                    Log.e(TAG, "run:toJson1: " + new Gson().toJson(bytes));
                } else {
                    bytes = inflate.etContent.getText().toString().getBytes();
                    Log.e(TAG, "run:toJson2: " + new Gson().toJson(bytes));
                }
                writeData(bytes);
                break;
            case R.id.btn_set_mtu:
                setmtu(0);
                break;
            case R.id.btn_select_notify:
//                characteristics

                break;
            case R.id.btn_select_write:

                break;
            case R.id.btn_clean:
                cleanTv();
                break;
            case R.id.btn_f001_h:
                writeDataF001(height);
                break;
            case R.id.btn_f001_l:
                writeDataF001(lower);
                break;
            case R.id.btn_f002_h:
                writeDataF002(height);
                break;
            case R.id.btn_f002_l:
                writeDataF002(lower);
                break;
        }
    }

    private void cleanTv() {
        inflate.tvLog.setText("");
        inflate.slScrollview.fullScroll(ScrollView.FOCUS_DOWN);
        sendSize=0;
        receviceSize=0;
        showSize();
    }

    void logtv(String text) {
        try {
            inflate.tvLog.append(text + "\t\n");
            inflate.slScrollview.fullScroll(ScrollView.FOCUS_DOWN);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    private void showSize() {
        inflate.tvSendSize.setText("" + sendSize);
        inflate.tvSendReceive.setText("" + receviceSize);
    }
}