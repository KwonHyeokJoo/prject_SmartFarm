package com.example.smartfarm;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.Switch;

import androidx.appcompat.app.AppCompatActivity;

public class SettingHeatActivity extends AppCompatActivity {

    String urlSetHeat = "http://192.168.10.108/setHeatC/";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.setting_heat);
        Switch heatSwitch;
        Button btnReturn4;

        heatSwitch = (Switch)findViewById(R.id.heatSwitch);
        heatSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if (heatSwitch.isChecked()==true) {
                    urlSetHeat = "http://192.168.10.108/setHeatC/1";
                    ((MainActivity)MainActivity.context_main).request(urlSetHeat);
                } else {
                    urlSetHeat = "http://192.168.10.108/setHeatC/0";
                    ((MainActivity)MainActivity.context_main).request(urlSetHeat);
                }
            }
        });

        btnReturn4 = (Button) findViewById(R.id.btnReturn4);
        btnReturn4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent outIntent = new Intent(getApplicationContext(), MainActivity.class);
                finish();
            }
        });
    }
}
