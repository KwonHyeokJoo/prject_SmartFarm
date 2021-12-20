package com.example.smartfarm;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.util.Log;
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
    TextView textView, btnTmp, btnHum, btnFan, btnHeat, btnLeftDoor, btnRightDoor;
    ImageView imageView;
    Button button;
    Switch control;
    public boolean stTemOn = false;
    public boolean stHumOn = false;

    Handler handler = new Handler();
    int targetTem, targetHum;
    boolean ConnectionState;
    public static Context context_main;
    public int curTem1, curTem2, curHum1, curHum2, leftWindow, rightWindow, fanState, heaterState,
            getCurTem1, getCurTem2, getCurHum1, getCurHum2, getLw, getRw, getFs, getHs;

    String urlCtrl = "http://192.168.10.108/setControl/";
    String urlTrd = "http://192.168.10.108/getState";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setTitle("SmartFarm");

        context_main = this;

        targetTem = 0;
        targetHum = 0;
        curTem1 = 0;
        curTem2 = 0;
        curHum1 = 0;
        curHum2 = 0;

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
        TextView btnFan = findViewById(R.id.btnFan);
        btnFan.setOnClickListener(new View.OnClickListener() {
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
                Intent intent = new Intent(getApplicationContext(), SettingHeatActivity.class);
                startActivity(intent);
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
                Intent intent = new Intent(getApplicationContext(), SettingWindowActivity.class);
                startActivity(intent);
            }
        });

        // 우측 개폐기
        TextView btnRightDoor = findViewById(R.id.btnRightDoor);
        btnRightDoor.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(getApplicationContext(), SettingWindowActivity.class);
                startActivity(intent);
            }
        });

        btnHum.setClickable(false);
        btnFan.setClickable(false);
        btnHeat.setClickable(false);
        btnLeftDoor.setClickable(false);
        btnRightDoor.setClickable(false);


        //스위치
        Switch control = findViewById(R.id.control);
        control.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                if (control.isChecked()==true) {
                    btnHum.setClickable(true);
                    btnFan.setClickable(true);
                    btnHeat.setClickable(true);
                    btnLeftDoor.setClickable(true);
                    btnRightDoor.setClickable(true);
                    urlCtrl += "1";
                    request(urlCtrl);
                } else {
                    btnHum.setClickable(false);
                    btnFan.setClickable(false);
                    btnHeat.setClickable(false);
                    btnLeftDoor.setClickable(false);
                    btnRightDoor.setClickable(false);
                    urlCtrl += "0";
                    request(urlCtrl);
                }

            }
        });

        new Thread(new Runnable() {
            @Override
            public void run() {
                while(true) {
                    try {
                        SystemClock.sleep(1000);
                        getState(urlTrd);
                        //서버로부터 받아온 값이 기존값과 다를 경우 세팅 : 온도
                        if(curTem1 != getCurTem1 || curTem2 != getCurTem2){
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    btnTmp.setText("현재 온도\n센서1 : " + curTem1 + "\n센서2 : " + curTem2 + "\n목표 온도 : " + targetTem);
                                    if(stTemOn == true){
                                        ((SettingTemActivity)SettingTemActivity.context_setTem).tvTem.
                                                setText("센서1 : "+ Integer.toString(curTem1) + "도\n센서2 : " + Integer.toString(curTem2) + "도\n");
                                    };
                                }
                            });
                            getCurTem1 = curTem1;
                            getCurTem2 = curTem2;
                        }

                        //서버로부터 받아온 값이 기존값과 다를 경우 세팅 : 습도
                        if(curHum1 != getCurHum1 || curHum2 != getCurHum2){
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    btnHum.setText("현재 습도\n센서1 : " + curHum1 + "\n센서2 : " + curHum2 + "\n목표 습도 : " + targetHum);
                                    if(stHumOn == true){
                                        ((SettingHumActivity)SettingHumActivity.context_setHum).tvHum.
                                                setText("센서1 : "+ Integer.toString(curHum1) + "도\n센서2 : " + Integer.toString(curHum2) + "도\n");
                                    };
                                }
                            });
                            getCurHum1 = curHum1;
                            getCurHum2 = curHum2;
                        }

                        //서버로부터 받아온 값이 기존값과 다를 경우 세팅 : 좌측개폐기
                        if(getLw != leftWindow){
                            switch (leftWindow){
                                case 0 :
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            btnLeftDoor.setText("좌측개폐기\n닫힘");
                                            btnLeftDoor.setBackgroundColor(Color.RED);
                                        }
                                    });

                                case 1 :
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            btnLeftDoor.setText("좌측개폐기\n열림");
                                            btnLeftDoor.setBackgroundColor(Color.BLUE);
                                        }
                                    });
                            }
                            getLw = leftWindow;
                        }

                        //서버로부터 받아온 값이 기존값과 다를 경우 세팅 : 우측개폐기
                        if(getRw != rightWindow){
                            switch (rightWindow){
                                case 0 :
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            btnRightDoor.setText("우측개폐기\n닫힘");
                                            btnRightDoor.setBackgroundColor(Color.RED);
                                        }
                                    });

                                case 1 :
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            btnRightDoor.setText("우측개폐기\n열림");
                                            btnRightDoor.setBackgroundColor(Color.BLUE);
                                        }
                                    });
                            }
                            getRw = rightWindow;
                        }

                        //서버로부터 받아온 값이 기존값과 다를 경우 세팅 : 팬
                        if(getFs != fanState){
                            switch (fanState){
                                case 0:
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            btnFan.setText("팬\n정지");
                                            btnFan.setBackgroundColor(Color.RED);
                                        }
                                    });
                                    break;
                                case 1:
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            btnFan.setText("팬\n가동");
                                            btnFan.setBackgroundColor(Color.BLUE);
                                        }
                                    });
                                    break;
                            }
                            getFs = fanState;
                        }
                        //서버로부터 받아온 값이 기존값과 다를 경우 세팅 : 열풍기
                        if(getHs != heaterState){
                            switch(heaterState){
                                case 0 :
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            btnHeat.setText("열풍기\n정지");
                                            btnHeat.setBackgroundColor(Color.RED);
                                        }
                                    });
                                    break;
                                case 1:
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            btnHeat.setText("열풍기\n가동");
                                            btnHeat.setBackgroundColor(Color.BLUE);
                                        }
                                    });
                                    break;
                            }
                            getHs = heaterState;
                        }
                        Thread.interrupted();
                    } catch (Exception e) {
                        e.printStackTrace();
                        Thread.interrupted();
                    }
                }
            }
        }).start();
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
            Log.d("test" , "Error " + ex.getMessage());
            ConnectionState = false;
        }
        if (ConnectionState == true) {
            String state = output.toString();
            String stateArray[] = state.split("/");

            curTem1 = Integer.parseInt(stateArray[0]);
            curTem2 = Integer.parseInt(stateArray[1]);
            curHum1 = Integer.parseInt(stateArray[2]);
            curHum2 = Integer.parseInt(stateArray[3]);
            leftWindow  = Integer.parseInt(stateArray[4]);
            rightWindow  = Integer.parseInt(stateArray[5]);
            fanState  = Integer.parseInt(stateArray[6]);
            heaterState  = Integer.parseInt(stateArray[7]);
        }
    }

    public void request(String urlStr) {
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
            Log.d("test" , "목표 온도를 다시 설정해 주세요.\nError: " + ex.getMessage());
        }
    }
}