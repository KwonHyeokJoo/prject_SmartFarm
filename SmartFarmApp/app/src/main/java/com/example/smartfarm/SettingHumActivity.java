package com.example.smartfarm;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.NumberPicker;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class SettingHumActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.setting_hum);

        Intent intent = getIntent();
        int targetHum = intent.getIntExtra("TargetHum", 30);    // 목표 습도
        int CurrentHum1 = intent.getIntExtra("CurrentHum1",30);   // 현재 습도1
        int CurrentHum2 = intent.getIntExtra("CurrentHum2",30);   // 현재 습도2

        TextView t1, tvTargetH, tvCurH, tvHum;// 습도
        Button btnReturn2, btnHumDown, btnHumUp;
        NumberPicker npcTarHum;

        tvHum = (TextView) findViewById(R.id.tvHum);
        tvHum.setText(Integer.toString(CurrentHum1) + "%\n" + Integer.toString(CurrentHum2) + "%\n");


        tvTargetH = (TextView) findViewById(R.id.tvTargetH);

        // 목표 습도값을 설정
        npcTarHum = (NumberPicker) findViewById(R.id.npcTarHum);
        npcTarHum.setMinValue(0);       // 습도의 최솟값이 0
        npcTarHum.setMaxValue(100);      // 습도의 최댓값이 100
        npcTarHum.setValue(targetHum);
        /*npcTarHum.setOnValueChangedListener(new NumberPicker.OnValueChangeListener() {
            @Override
            public void onValueChange(NumberPicker picker, int oldVal, int newVal) {
                targetHum = newVal;
            }
        });*/

        // + 버튼을 누르면 값이 1씩 증가
        btnHumUp = (Button) findViewById(R.id.btnHumUp);
        btnHumUp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                npcTarHum.setValue(npcTarHum.getValue()+1);
            }
        });

        // - 버튼을 누르면 값이 1씩 감소
        btnHumDown = (Button) findViewById(R.id.btnHumDown);
        btnHumDown.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                npcTarHum.setValue(npcTarHum.getValue()-1);
            }
        });


        // 종료 버튼
        btnReturn2 = (Button) findViewById(R.id.btnReturn2);
        btnReturn2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent outIntent = new Intent(getApplicationContext(), MainActivity.class);
                outIntent.putExtra("TargetTemp", targetHum);
                setResult(RESULT_OK, outIntent);
                finish();
            }
        });
    }
}