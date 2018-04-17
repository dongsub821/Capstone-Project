package com.example.caucse.db;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ImageView;

import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class SourceViewActivity extends AppCompatActivity {


    int imageName;
    Bitmap bmImg;
    ImageView imView;
    String imgUrl = "http://teamssdweb.kr/Cam_image/";
    String url = "http://teamssdweb.kr/DB/DB.php";
    back task;
    ImageView iv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_source_view);
        imageName = getIntent().getIntExtra("imageName", 1);
        task = new back();
        task.execute(imgUrl+imageName+".png");

        iv = (ImageView) findViewById(R.id.imageView);

        iv.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent intent = new Intent(SourceViewActivity.this, MainActivity.class);
                startActivity(intent);
            }
        });


    }
    private class back extends AsyncTask<String, Integer,Bitmap>{

        @Override
        protected Bitmap doInBackground(String... urls) {
            // TODO Auto-generated method stub
            try{
                //  ArrayList<String> str =new ArrayList<String>();
                //StringTokenizer tokens = new StringTokenizer(urls.toString(),"*");
                URL myFileUrl = new URL(urls[0]);
                HttpURLConnection conn = (HttpURLConnection)myFileUrl.openConnection();
                conn.setDoInput(true);
                conn.connect();
                InputStream is = conn.getInputStream();
                bmImg = BitmapFactory.decodeStream(is);
                // num = Integer.parseInt(tokens.nextToken());
            }catch(IOException e){
                e.printStackTrace();
            }
            return bmImg;
        }

        protected void onPostExecute(Bitmap img){
           iv.setImageBitmap(bmImg);
        }

    }


}
