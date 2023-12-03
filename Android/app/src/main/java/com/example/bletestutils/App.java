package com.example.bletestutils;

import android.app.Application;

import com.example.bletestutils.utils.FastBleUtils;
import com.tencent.mmkv.MMKV;

public class App extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        MMKV.initialize(getApplicationContext());
        FastBleUtils.getInstant().init(this);
    }
}
