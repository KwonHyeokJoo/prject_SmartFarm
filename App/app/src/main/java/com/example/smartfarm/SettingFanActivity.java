package com.example.smartfarm;

import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

public class SettingFanActivity extends AppCompatActivity {

    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.setting_fan);

        // 팬 속도 제어 및 속도 표시
        final TextView tvTargetF = (TextView) findViewById(R.id.tvTargetF);
        SeekBar SBfanSpeed = (SeekBar) findViewById(R.id.SBfanSpeed);
        Button btnReturn3;
        SBfanSpeed.setMax(255);

        SBfanSpeed.setClickable(false);

        Switch fanSwitch = findViewById(R.id.fanSwitch);
        fanSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                if (fanSwitch.isChecked()==true) {
                    SBfanSpeed.setClickable(true);

                } else {
                    SBfanSpeed.setClickable(false);
                }
            }
        });


        SBfanSpeed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                tvTargetF.setText("팬 속도 : " + (i * 23.5) + "rpm");

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }

        });

        btnReturn3 = (Button) findViewById(R.id.btnReturn3);
        btnReturn3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent outIntent = new Intent(getApplicationContext(), MainActivity.class);
                finish();
            }
        });

    }
}
