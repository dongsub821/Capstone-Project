package com.example.caucse.myapplication;


import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.ListView;


public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ListView listview ;
        ListViewAdapter adapter;

        // Adapter 생성
        adapter = new ListViewAdapter() ;

        // 리스트뷰 참조 및 Adapter달기
        listview = (ListView) findViewById(R.id.listview1);
        listview.setAdapter(adapter);

        // 첫 번째 아이템 추가.
        adapter.addItem(ContextCompat.getDrawable(this, R.drawable.a),    "Box", "Account Box Black 36dp") ;
        // 두 번째 아이템 추가.
        adapter.addItem(ContextCompat.getDrawable(this, R.drawable.as),    "Circle", "Account Circle Black 36dp") ;
        // 세 번째 아이템 추가.
        adapter.addItem(ContextCompat.getDrawable(this, R.drawable.asd),    "Ind", "Assignment Ind Black 36dp") ;
    }


}
