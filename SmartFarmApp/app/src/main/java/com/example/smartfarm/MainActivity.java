package com.example.smartfarm;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {
    EditText editText;
    TextView textView, btnTmp, btnHum, btnGrd, btnHeat, btnLeftDoor, btnRightDoor;
    ImageView imageView;
    Button button;
    Switch control;

    Handler handler = new Handler();
    int targetTem, targetHum;
    int curTem1, curTem2, curHum1, curHum2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setTitle("SmartFarm");

        targetTem = 0;
        targetHum = 0;
        curTem1 = 0;
        curTem2 = 0;
        curHum1 = 0;
        curHum2 = 0;


        final String urlStr = "http://192.168.0.26/getCurrentData"; //현재 상태 받아오기 url

        /*new Thread(new Runnable() {
            @Override
            public void run() {
                getState(urlStr);
                btnTmp.setText("현재 온도 센서1 : " + curTem1 + "\n센서2 : " + curTem2 + "\r목표 온도 : " + targetTem);
                btnHum.setText("현재 습도 센서1 : " + curHum1 + "\n센서2 : " + curHum2 + "\r목표 온도 : " + targetHum);
            }
        }).start();*/

        // 온도
        TextView btnTmp = findViewById(R.id.btnTmp);
        btnTmp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /*final String urlStr = "http://192.168.1.1/tmp/36.5"; //editText.getText().toString();

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        request(urlStr);
                    }
                }).start(); */
                Intent intent = new Intent(getApplicationContext(), SettingTemActivity.class);
                intent.putExtra("TargetTem", targetTem);
                intent.putExtra("CurrentTem1", curTem1);
                intent.putExtra("CurrentTem2", curTem2);
                startActivityForResult(intent,0);
            }
        });

        // 습도
        TextView btnHum = findViewById(R.id.btnHum);
        btnHum.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /* final String urlStr = "http://192.168.1.1/hum/60"; //editText.getText().toString();

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        request(urlStr);
                    }
                }).start(); */

                Intent intent = new Intent(getApplicationContext(), SettingHumActivity.class);
                intent.putExtra("TargetHum", targetHum);
                intent.putExtra("CurrentHum1", curHum1);
                intent.putExtra("CurrentHum2", curHum2);
                startActivityForResult(intent,0);
            }
        });

        // 팬
        TextView btnGrd = findViewById(R.id.btnGrd);
        btnGrd.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
               /* final String urlStr = "http://192.168.1.1/grd/30"; //editText.getText().toString();

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        request(urlStr);
                    }
                }).start(); `*/
                Intent intent = new Intent(getApplicationContext(), SettingFanActivity.class);
                startActivity(intent);
            }
        });

        // 열풍기
        TextView btnHeat = findViewById(R.id.btnHeat);
        btnHeat.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /* final String urlStr = "http://192.168.1.1/gettmp"; //editText.getText().toString();

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        request(urlStr);
                    }
                }).start(); */
            }
        });

        // 좌측 개폐기
        TextView btnLeftDoor = findViewById(R.id.btnLeftDoor);
        btnLeftDoor.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /*final String urlStr = "http://192.168.1.1/btnLeftDoor"; //editText.getText().toString();

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        request(urlStr);
                    }
                }).start();*/
            }
        });

        // 우측 개폐기
        TextView btnRightDoor = findViewById(R.id.btnRightDoor);
        btnRightDoor.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /*final String urlStr = "http://192.168.1.1/btnRightDoor"; //editText.getText().toString();

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        request(urlStr);
                    }
                }).start();*/
            }
        });

        btnTmp.setClickable(false);
        btnHum.setClickable(false);
        btnGrd.setClickable(false);
        btnHeat.setClickable(false);
        btnLeftDoor.setClickable(false);
        btnRightDoor.setClickable(false);

        //스위치
        Switch control = findViewById(R.id.control);
        control.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                if (control.isChecked()==true) {
                    btnTmp.setClickable(true);
                    btnHum.setClickable(true);
                    btnGrd.setClickable(true);
                    btnHeat.setClickable(true);
                    btnLeftDoor.setClickable(true);
                    btnRightDoor.setClickable(true);
                } else {
                    btnTmp.setClickable(false);
                    btnHum.setClickable(false);
                    btnGrd.setClickable(false);
                    btnHeat.setClickable(false);
                    btnLeftDoor.setClickable(false);
                    btnRightDoor.setClickable(false);
                }

            }
        });

        Timer timer = new Timer();

        TimerTask TT = new TimerTask() {
            @Override
            public void run() {
                // 반복실행할 구문
                targetTem++;
                targetHum++;
                curTem1++;
                curTem2++;
                curHum1++;
                curHum2++;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        btnTmp.setText("현재 온도\n센서1 : " + curTem1 + "\n센서2 : " + curTem2 + "\n목표 온도 : " + targetTem);
                        btnHum.setText("현재 습도\n센서1 : " + curHum1 + "\n센서2 : " + curHum2 + "\n목표 온도 : " + targetHum);
                    }
                });
            }
        };

        timer.schedule(TT, 0, 1000); //Timer 실행
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(resultCode == RESULT_OK){
            targetTem = data.getIntExtra("TargetTemp", 0);
        }
    }

    // 서버 요청하기
    public void getState(String urlStr) {
        StringBuilder output = new StringBuilder();
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
                reader.close();
                conn.disconnect();
            }
        } catch (Exception ex) {
            Toast.makeText(getApplicationContext(), "오류 발생", Toast.LENGTH_SHORT).show();
        }
        String state = output.toString();
        String stateArray[] = state.split("/");

        curTem1 = Integer.parseInt(stateArray[0]);
        curTem2 = Integer.parseInt(stateArray[1]);
        curHum1 = Integer.parseInt(stateArray[2]);
        curHum2 = Integer.parseInt(stateArray[3]);
    }
}