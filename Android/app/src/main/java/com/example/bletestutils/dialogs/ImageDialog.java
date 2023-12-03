package com.example.bletestutils.dialogs;

import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;

import androidx.annotation.NonNull;

import com.example.bletestutils.BaseActivity;
import com.example.bletestutils.databinding.ImageDialogBinding;

public class ImageDialog extends BaseDialog{

    private com.example.bletestutils.databinding.ImageDialogBinding inflate;
    int imageRes=-1;

    public int getImageRes() {
        return imageRes;
    }

    public ImageDialog setImageRes(int imageRes) {
        this.imageRes = imageRes;
        return this;
    }

    public ImageDialog(@NonNull BaseActivity context) {
        super(context);
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        inflate = ImageDialogBinding.inflate(getLayoutInflater());
        setContentView(inflate.getRoot());
        Window window = getWindow();
        WindowManager.LayoutParams attributes = window.getAttributes();
        int screenWidth = baseActivity.getWindowManager().getDefaultDisplay().getWidth();
        attributes.width = screenWidth * 95 / 100;
        attributes.height = screenWidth * 95 / 100;
//        attributes.dimAmount = 0f;
        if (imageRes!=-1)
            inflate.ivContent.setBackgroundResource(imageRes);
    }
}
