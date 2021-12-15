package com.example.smartfarm;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.NumberPicker;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class SettingTemActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.setting_tem);

        Intent intent = getIntent();
        int targetTem = intent.getIntExtra("TargetTem", 30);    // 목표 온도
        int CurrentTem1 = intent.getIntExtra("CurrentTem1",30);   // 현재 온도1
        int CurrentTem2 = intent.getIntExtra("CurrentTem2",30);   // 현재 온도2

        TextView t1, tvTargetT, tvCurT, tvTem;// 온도
        Button btnReturn1, btnTmpDown, btnTmpUp;
        NumberPicker npcTarTmp;

        tvTem = (TextView) findViewById(R.id.tvTem);
        tvTem.setText(Integer.toString(CurrentTem1) + "도\n" + Integer.toString(CurrentTem2) + "도\n");


        tvTargetT = (TextView) findViewById(R.id.tvTargetT);

        // 목표 온도값을 설정
        npcTarTmp = (NumberPicker) findViewById(R.id.npcTarTmp);
        npcTarTmp.setMinValue(0);   // 온도의 최소값 0도
        npcTarTmp.setMaxValue(50);  // 온도의 최댓값 50도
        npcTarTmp.setValue(targetTem);
        /*npcTarTmp.setOnValueChangedListener(new NumberPicker.OnValueChangeListener() {
            @Override
            public void onValueChange(NumberPicker picker, int oldVal, int newVal) {
                targetTem = newVal;
            }
        });*/

        // + 버튼을 누르면 값이 1씩 증가
        btnTmpUp = (Button) findViewById(R.id.btnTmpUp);
        btnTmpUp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                npcTarTmp.setValue(npcTarTmp.getValue()+1);
            }
        });

        // - 버튼을 누르면 값이 1씩 감소
        btnTmpDown = (Button) findViewById(R.id.btnTmpDown);
        btnTmpDown.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                npcTarTmp.setValue(npcTarTmp.getValue()-1);
            }
        });

        // 종료 버튼
        btnReturn1 = (Button) findViewById(R.id.btnReturn1);
        btnReturn1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent outIntent = new Intent(getApplicationContext(), MainActivity.class);
                outIntent.putExtra("TargetTemp", targetTem);
                setResult(RESULT_OK, outIntent);
                finish();
            }
        });

        // 현재 온도
    }
}
