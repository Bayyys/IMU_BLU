package com.example.bletestutils.dialogs;

import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

import androidx.annotation.NonNull;

import com.example.bletestutils.BaseActivity;
import com.example.bletestutils.R;


public abstract class ConfirmDialog extends BaseDialog implements View.OnClickListener {

    TextView tvTips;


    TextView btnCencel;

    TextView btnSure;
    String tips;

    public String getTips() {
        return tips;
    }

    public ConfirmDialog setTips(String tips) {
        this.tips = tips;
        return this;
    }

    public ConfirmDialog(@NonNull BaseActivity context) {
        super(context);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.confirm_dialog);
        Window window = getWindow();
        WindowManager.LayoutParams attributes = window.getAttributes();
        int screenWidth = baseActivity.getWindowManager().getDefaultDisplay().getWidth();

        attributes.width = screenWidth * 4 / 5;
        attributes.height = screenWidth * 1 / 2;
        attributes.dimAmount = 0f;
        tvTips = findViewById(R.id.tv_tips);
        btnCencel = findViewById(R.id.btn_cencel);
        btnSure = findViewById(R.id.btn_sure);
        if (!TextUtils.isEmpty(tips)) tvTips.setText(tips);

        btnCencel.setOnClickListener(this);
        btnSure.setOnClickListener(this);
    }

    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btn_cencel:
                btnCancel();
                break;
            case R.id.btn_sure:
                btnSure();
                break;
        }
        dismiss();
    }

    public abstract void btnSure();

    protected abstract void btnCancel();
}
