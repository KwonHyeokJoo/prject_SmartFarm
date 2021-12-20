package com.example.smartfarm;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class SettingWindowActivity extends AppCompatActivity {

    String urlSetLeftWindow = "http://192.168.10.108/setLeftWindowC/";
    String urlSetRightWindow = "http://192.168.10.108/setRightWindowC/";

    @Override
    protected void onCreate( Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.setting_window);

        Switch lwSwitch, rwSwitch;
        Button btnReturn5;

        lwSwitch = (Switch)findViewById(R.id.lwSwitch);
        lwSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if (lwSwitch.isChecked()==true) {
                    urlSetLeftWindow = "http://192.168.10.108/setLeftWindowC/1";
                    ((MainActivity)MainActivity.context_main).request(urlSetLeftWindow);
                } else {
                    urlSetLeftWindow = "http://192.168.10.108/setLeftWindowC/0";
                    ((MainActivity)MainActivity.context_main).request(urlSetLeftWindow);
                }
            }
        });

        rwSwitch = (Switch)findViewById(R.id.rwSwitch);
        rwSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if (rwSwitch.isChecked()==true) {
                    urlSetRightWindow = "http://192.168.10.108/setRightWindowC/1";
                    ((MainActivity)MainActivity.context_main).request(urlSetRightWindow);
                } else {
                    urlSetRightWindow = "http://192.168.10.108/setRightWindowC/0";
                    ((MainActivity)MainActivity.context_main).request(urlSetRightWindow);
                }
            }
        });

        btnReturn5 = (Button) findViewById(R.id.btnReturn5);
        btnReturn5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent outIntent = new Intent(getApplicationContext(), MainActivity.class);
                finish();
            }
        });
    }
}
