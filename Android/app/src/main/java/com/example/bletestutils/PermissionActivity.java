package com.example.bletestutils;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.util.Log;
import android.widget.Toast;

import com.example.bletestutils.BaseActivity;

import java.util.ArrayList;
import java.util.List;

import pub.devrel.easypermissions.EasyPermissions;

public class PermissionActivity extends BaseActivity {
    int PERMISSION_REQUESTCODE=12;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_permission);
        initPermission();
    }

    // 动态申请权限
    private void initPermission() {
         ArrayList<String> permissionLists=new ArrayList<>();
        // Android 版本大于等于 Android12 时
        // 只包括蓝牙这部分的权限，其余的需要什么权限自己添加
        permissionLists.add(Manifest.permission.BLUETOOTH_SCAN);
        permissionLists.add(Manifest.permission.BLUETOOTH_ADVERTISE);
        permissionLists.add(Manifest.permission.BLUETOOTH_CONNECT);
        // Android 版本小于 Android12 及以下版本
        permissionLists.add(Manifest.permission.ACCESS_COARSE_LOCATION);
        permissionLists.add(Manifest.permission.ACCESS_FINE_LOCATION);
        if (permissionLists.size() > 0) {
            ActivityCompat.requestPermissions(this, permissionLists.toArray(new String[permissionLists.size()]), PERMISSION_REQUESTCODE);
        }else{
            startActivity(MainActivity.class);
            finish();
        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
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
                startActivity(MainActivity.class);
                finish();
            } else {
              //权限还么有照样去了！
                startActivity(MainActivity.class);
                finish();
            }
        }
        ////////
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1024 && Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            // 检查是否有权限
            if (Environment.isExternalStorageManager()) {
                // 授权成功
                startActivity(MainActivity.class);
                finish();
            } else {
                // 授权失败
             toastText("读写权限获取失败！");
            }
        }
    }
}