package com.example.bletestutils.dialogs;


import android.app.Dialog;
import android.os.Bundle;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;

import androidx.annotation.NonNull;

import com.example.bletestutils.BaseActivity;
import com.example.bletestutils.R;


public class BaseDialog extends Dialog  {
    String TAG=this.getClass().getSimpleName();
    int gravity= Gravity.CENTER;
    BaseActivity baseActivity;

    public void showLeft(){
        this.gravity= Gravity.LEFT;
        show();
    }
    public void showRight(){
        this.gravity= Gravity.RIGHT;
        show();
    }
    public void showBottom(){
        this.gravity= Gravity.BOTTOM;
        show();
    }
    public void showTop(){
        this.gravity= Gravity.TOP;
        show();
    }

    @Override
    public void show() {
        if (baseActivity!=null&&!baseActivity.isFinishing())
        super.show();
    }

    public BaseDialog(@NonNull BaseActivity context) {
        super(context, R.style.custom_dialog);
        baseActivity=context;
    }
    public View getViewHolderView(int viewId, ViewGroup parent){
        return LayoutInflater.from(getContext()).inflate(viewId,parent,false);
    }
    @Override
    public void setContentView(int layoutResID) {
        super.setContentView(layoutResID);

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Window window = getWindow();
        window.setGravity(gravity);
    }

    public   View getRootView() {
        return getWindow().getDecorView().findViewById(android.R.id.content);
    }
}
