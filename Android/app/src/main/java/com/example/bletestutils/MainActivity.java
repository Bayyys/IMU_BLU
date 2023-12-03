package com.example.bletestutils;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.SimpleItemAnimator;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;


import com.clj.fastble.callback.BleScanCallback;
import com.clj.fastble.data.BleDevice;
import com.example.bletestutils.databinding.ActivityMainBinding;
import com.example.bletestutils.databinding.DevItemBinding;
import com.example.bletestutils.dialogs.ImageDialog;
import com.example.bletestutils.utils.FastBleUtils;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class MainActivity extends BaseActivity implements View.OnClickListener {
    private ActivityMainBinding inflate;
    private ArrayList<BleDevice> allDevices;
    private ArrayList<BleDevice> bleDevices;
    private ImageDialog imageDialog;
    Runnable command = new Runnable() {
        @Override
        public void run() {
            notifyList();
        }
    };
    ScheduledExecutorService scheduledThreadPool = Executors.newScheduledThreadPool(1);
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        inflate = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(inflate.getRoot());
        inflate.btnScan.setOnClickListener(this);
        inflate.btnShowQrc.setOnClickListener(this);
        allDevices = new ArrayList<>();
        bleDevices = new ArrayList<>();
        initList();
        startBleScan(0);
        scheduledThreadPool.scheduleAtFixedRate(command,1000,1000, TimeUnit.MILLISECONDS);
    }


    private void initList() {
        //关闭动画效果
        SimpleItemAnimator sa = (SimpleItemAnimator) inflate.rvDevList.getItemAnimator();
        sa.setSupportsChangeAnimations(false);
//设置动画为空
        inflate.rvDevList.setItemAnimator(null);
        inflate.rvDevList.setLayoutManager(new LinearLayoutManager(getThis()));
        inflate.rvDevList.setAdapter(new RecyclerView.Adapter() {
            @NonNull
            @Override
            public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
                return new DevViewHolder(LayoutInflater.from(getThis()).inflate(R.layout.dev_item, parent, false));
            }

            @Override
            public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
                DevViewHolder devViewHolder = (DevViewHolder) holder;
                BleDevice bleDevice = bleDevices.get(position);
                devViewHolder.bind.tvName.setText(bleDevice.getName());
                devViewHolder.bind.tvMac.setText(bleDevice.getMac());
                int rssiValue = bleDevice.getRssi();
                devViewHolder.bind.tvRssi.setText("rssi:" + rssiValue);
                devViewHolder.itemView.setTag(bleDevice);
//                if (rssiValue>rssi){
//                    devViewHolder.itemView.setVisibility(View.VISIBLE);
//                } else{
//                    devViewHolder.itemView.setVisibility(View.GONE);
//                }

            }

            @Override
            public int getItemCount() {
                return bleDevices == null ? 0 : bleDevices.size();
            }
        });
    }

    @Override
    public void onClick(View v) {
        if (!isFastClick())
            switch (v.getId()) {
                case R.id.btn_scan:
                    startBleScan(0);
                    break;
                case R.id.btn_show_qrc:
                    showQrc();
                    break;
            }
    }

    private void showQrc() {
        if (imageDialog == null)
            imageDialog = new ImageDialog(getThis()).setImageRes(R.mipmap.qrc_image);
        if (!imageDialog.isShowing())
            imageDialog.show();
    }

    Handler sH = new Handler();

    public void startBleScan(long delay) {
        sH.removeCallbacksAndMessages(null);
        try {
            FastBleUtils.getInstant().cancelScan();
        } catch (Exception e) {
            Log.e(TAG, "startBleScan: " + e);
        }
        allDevices.clear();
        bleDevices.clear();
        notifyList();
        sH.postDelayed(new Runnable() {
            @Override
            public void run() {
                Log.e(TAG, "run: startBleScan:");

                FastBleUtils.getInstant().scanBle(new BleScanCallback() {
                    @Override
                    public void onScanFinished(List<BleDevice> scanResultList) {

                    }

                    @Override
                    public void onScanStarted(boolean success) {

                    }

                    @Override
                    public void onLeScan(BleDevice bleDevice) {
                        super.onLeScan(bleDevice);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                synchronized (MainActivity.this){
                                    addDev(bleDevice);
                                }
                            }
                        });
                    }
                    @Override
                    public void onScanning(BleDevice bleDevice) {
//                        addDev(bleDevice);
//                        runOnUiThread(new Runnable() {
//                            @Override
//                            public void run() {
//                                inflate.rvDevList.getAdapter().notifyDataSetChanged();
//                            }
//                        });
                    }
                });
            }
        }, delay);
    }

    private synchronized void addDev(BleDevice bleDevice) {
        for (BleDevice b : allDevices) {
            if (b.getMac().equals(bleDevice.getMac())) {
                int i = allDevices.indexOf(b);
                allDevices.remove(i);
                allDevices.add(i, bleDevice);
//                notifyList();
                return;
            }
        }
        allDevices.add(bleDevice);
//       notifyList();
    }
    void comparatorData(){
        Comparator<BleDevice> bleDeviceComparator = new Comparator<BleDevice>() {
            @Override
            public int compare(BleDevice o1, BleDevice o2) {
                return o2.getRssi()-o1.getRssi();
            }

        };
        Collections.sort(bleDevices, bleDeviceComparator);
    }
    private synchronized void notifyList() {
       runOnUiThread(new Runnable() {
           @Override
           public void run() {
             synchronized (MainActivity.this){
                 bleDevices.clear();
                 bleDevices.addAll(allDevices);
                 comparatorData();
                 inflate.rvDevList.getAdapter().notifyDataSetChanged();
             }
           }
       });
    }

    class DevViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {

        private final com.example.bletestutils.databinding.DevItemBinding bind;

        public DevViewHolder(@NonNull View itemView) {
            super(itemView);
            bind = DevItemBinding.bind(itemView);
            itemView.setOnClickListener(this);
        }

        @Override
        public void onClick(View v) {
            if (isFastClick()) return;
            BleDevice bleDevice = (BleDevice) v.getTag();
            Intent intent = new Intent(getThis(), ControlActivity.class);
            intent.putExtra("dev", bleDevice);
            startActivity(intent);
        }
    }
}