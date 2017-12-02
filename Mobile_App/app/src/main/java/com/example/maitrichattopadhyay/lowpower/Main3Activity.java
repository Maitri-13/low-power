package com.example.maitrichattopadhyay.lowpower;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;

public class Main3Activity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main3);

        Intent intent = getIntent();
        String newText =  intent.getStringExtra("json");
        TextView textView2 = (TextView) findViewById(R.id.textView3);
        Log.i("new text " , "a" + newText);
        textView2.setText(newText);
    }
}
