package com.example.bletestutils;

import android.Manifest;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.PersistableBundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.List;

public class BaseActivity extends AppCompatActivity {
    String TAG =getClass().getSimpleName();
    long beforeClickTime;
    private int PERMISSION_REQUESTCODE=12;
    private Toast toast;



    public BaseActivity getThis(){
        return this;
    }

    public void setScreenDirection(boolean vertical){
        if (vertical){
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        }else {
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        }
    }
    public void getPermission() {
        List<String> permissionLists = new ArrayList<>();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                permissionLists.add(Manifest.permission.BLUETOOTH_SCAN);
                Log.e(TAG, "getPermission: 5" );
            }
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_ADVERTISE) != PackageManager.PERMISSION_GRANTED) {
                permissionLists.add(Manifest.permission.BLUETOOTH_ADVERTISE);
                Log.e(TAG, "getPermission: 10" );
            }
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                permissionLists.add(Manifest.permission.BLUETOOTH_CONNECT);
                Log.e(TAG, "getPermission: 4" );
            }
        }else {
            // Android 版本小于 Android12 及以下版本

            if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                permissionLists.add(Manifest.permission.ACCESS_COARSE_LOCATION);
                Log.e(TAG, "getPermission: 1" );
            }
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                permissionLists.add(Manifest.permission.ACCESS_FINE_LOCATION);
                Log.e(TAG, "getPermission: 6" );
            }
        }

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH) != PackageManager.PERMISSION_GRANTED) {
            permissionLists.add(Manifest.permission.BLUETOOTH);
            Log.e(TAG, "getPermission: 2" );
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_ADMIN) != PackageManager.PERMISSION_GRANTED) {
            permissionLists.add(Manifest.permission.BLUETOOTH_ADMIN);
            Log.e(TAG, "getPermission: 3" );
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            permissionLists.add(Manifest.permission.READ_EXTERNAL_STORAGE);
            Log.e(TAG, "getPermission: 7" );
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            permissionLists.add(Manifest.permission.WRITE_EXTERNAL_STORAGE);
            Log.e(TAG, "getPermission: 8" );
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.INTERNET) != PackageManager.PERMISSION_GRANTED) {
            permissionLists.add(Manifest.permission.INTERNET);
            Log.e(TAG, "getPermission: 9" );
        }

        Log.e(TAG, "getPermission: "+ permissionLists.size());
        if (!permissionLists.isEmpty()) {//说明肯定有拒绝的权限
            Log.e(TAG, "getPermission:???: "+ permissionLists.size());
//            ActivityCompat.requestPermissions(this, permissionLists.toArray(new String[permissionLists.size()]), PERMISSION_REQUESTCODE);
        } else {

        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        //////////
        Log.e(TAG, "onRequestPermissionsResult: " );
        if(requestCode == PERMISSION_REQUESTCODE) {
            int grantCount = 0;
            for (int grant : grantResults) {
                if (grant == PackageManager.PERMISSION_GRANTED) {
                    grantCount++;
                }
            }
            if (grantCount == grantResults.length) {
                // 权限获取成功，执行后面的代码

            } else {
                // 权限获取失败  提示用户

            }
        }

        ////////
    }
    public boolean isFastClick(){
        long currentTimeMillis = System.currentTimeMillis();
        boolean b = currentTimeMillis - beforeClickTime < 500;
        Log.e(TAG, "isFastClick: "+b );
        beforeClickTime=currentTimeMillis;
        return b;
    }
    public void startActivity(Class activity){
        startActivity(new Intent(this,activity));
    }
    public void toastText(String msg){
            if (toast == null) {
                toast = Toast.makeText(this, msg, Toast.LENGTH_SHORT);
            } else {
                toast.setText(msg);
            }
            toast.show();
    }

    final char[] hexArray = "0123456789ABCDEF".toCharArray();


    public  String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for (int j = 0; j < bytes.length; j++) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = hexArray[v >>> 4];
            hexChars[j * 2 + 1] = hexArray[v & 0x0F];
        }
        return new String(hexChars);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN);
        setScreenDirection(true);
    }

    public void topWindow() {
        Window window = getWindow();
        //4.0版本及以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            window.setFlags(
                    WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS,
                    WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        }
        //5.0版本及以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS
                    | WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION);
            window.getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
            window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            window.setStatusBarColor(Color.TRANSPARENT);
            window.setNavigationBarColor(Color.TRANSPARENT);
        }
    }

}
