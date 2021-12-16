package com.example.smartfarm;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.NumberPicker;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class SettingTemActivity extends AppCompatActivity {
    boolean ConnectionState;
    public TextView tvTargetT, tvTem;// 온도
    Button btnReturn1, btnTmpDown, btnTmpUp;
    NumberPicker npcTarTmp;
    int CurrentTem1, CurrentTem2, targetTem;
    public static Context context_setTem;
    String urlsetTem;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.setting_tem);

        context_setTem = this;

        ((MainActivity)MainActivity.context_main).stTemOn = true;

        Intent intent = getIntent();
//        targetTem = intent.getIntExtra("TargetTem", 30);    // 목표 온도
//        CurrentTem1 = intent.getIntExtra("CurrentTem1",30);   // 현재 온도1
//        CurrentTem2 = intent.getIntExtra("CurrentTem2",30);   // 현재 온도2
        CurrentTem1 = ((MainActivity)MainActivity.context_main).curTem1;
        CurrentTem2 = ((MainActivity)MainActivity.context_main).curTem2;

        tvTem = (TextView) findViewById(R.id.tvTem);
        tvTem.setText("센서1 : "+ Integer.toString(CurrentTem1) + "도\n센서2 : " + Integer.toString(CurrentTem2) + "도\n");


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
                targetTem = npcTarTmp.getValue();
                outIntent.putExtra("TargetTemp", targetTem);
                setResult(RESULT_OK, outIntent);
                finish();
            }
        });
    }

    @Override
    protected void onPause() {
        super.onPause();
        ((MainActivity)MainActivity.context_main).stTemOn = false;
        urlsetTem = new String("192.168.0.26/setTargetTem/" + Integer.toString(targetTem));
        request(urlsetTem);
    }

    public void request(String urlStr) {
        StringBuilder output = new StringBuilder();
        ConnectionState = false;
        try {
            URL url = new URL(urlStr);
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            if (conn != null) {
                conn.setConnectTimeout(10000);
                conn.setRequestMethod("GET");
                conn.setDoInput(true);

                int resCode = conn.getResponseCode();

                BufferedReader reader = new BufferedReader(new InputStreamReader(conn.getInputStream()));
                String line = null;

                while (true) {
                    line = reader.readLine();
                    if (line == null) {
                        break;
                    }
                    output.append(line + "\n");

                }
                //reader.close();

                conn.disconnect();
                reader.close();
                ConnectionState = true;
            }
        } catch (Exception ex) {
            Log.d("test" , "목표 온도를 다시 설정해 주세요.\nError: " + ex.getMessage());
        }
    }
}
