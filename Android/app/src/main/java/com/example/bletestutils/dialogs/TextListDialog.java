package com.example.bletestutils.dialogs;

import android.os.Bundle;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.example.bletestutils.BaseActivity;
import com.example.bletestutils.R;
import com.example.bletestutils.databinding.TextItemBinding;
import com.example.bletestutils.databinding.TextListDialogBinding;

import java.util.ArrayList;

public abstract class TextListDialog extends BaseDialog {

    private com.example.bletestutils.databinding.TextListDialogBinding inflate;
    String title;

    public String getTitle() {
        return title;
    }

    public TextListDialog setTitle(String title) {
        this.title = title;
        return this;
    }

    ArrayList<String> datas;

    public ArrayList<String> getDatas() {
        return datas;
    }

    public TextListDialog setDatas(ArrayList<String> datas) {
        this.datas = datas;
        return this;
    }

    public TextListDialog(@NonNull BaseActivity context) {
        super(context);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        inflate = TextListDialogBinding.inflate(getLayoutInflater());
        setContentView(inflate.getRoot());
        Window window = getWindow();
        WindowManager.LayoutParams attributes = window.getAttributes();
        int screenWidth = baseActivity.getWindowManager().getDefaultDisplay().getWidth();
        attributes.width = screenWidth * 8 / 9;
        attributes.height = screenWidth;
        initList();
        if (!TextUtils.isEmpty(title))inflate.tvTitle.setText(title);
    }

    private void initList() {
        inflate.rvList.setLayoutManager(new LinearLayoutManager(baseActivity));
        inflate.rvList.setAdapter(new RecyclerView.Adapter() {
            @NonNull
            @Override
            public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
                return new TextViewHolder(LayoutInflater.from(baseActivity).inflate(R.layout.text_item,parent,false));
            }

            @Override
            public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
                TextViewHolder textViewHolder= (TextViewHolder) holder;
                textViewHolder.bind.tvContent.setText(datas.get(position));
                textViewHolder.itemView.setTag(position);
            }

            @Override
            public int getItemCount() {
                return datas==null?0:datas.size();
            }
        });
    }
    class TextViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {

        private final com.example.bletestutils.databinding.TextItemBinding bind;

        public TextViewHolder(@NonNull View itemView) {
            super(itemView);
            bind = TextItemBinding.bind(itemView);
            itemView.setOnClickListener(this);
        }

        @Override
        public void onClick(View v) {
            int position = (int) v.getTag();
            selectItem(position);
            dismiss();
        }
    }
    public abstract void selectItem(int position);
}
