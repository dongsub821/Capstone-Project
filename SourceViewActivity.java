package com.example.caucse.db0602;

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


    String imageName;
    Bitmap bmImg;
    ImageView imView;
    String imgUrl = "http://teamssdweb.kr/Cam_image/";
    String url = "http://teamssdweb.kr/DB/DB.php";
    back task;
    ImageView iv1,iv2,iv3,iv4;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_source_view);
        imageName = getIntent().getStringExtra("imageName");
        task = new back();
        task.execute(imgUrl+imageName+".jpg");

        iv1 = findViewById(R.id.imageView2);
        iv2 = findViewById(R.id.imageView3);
        iv3 = findViewById(R.id.imageView4);
        iv4 = findViewById(R.id.imageView5);

        iv1.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                finish();
            }
        });
        iv2.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                finish();
            }
        });
        iv3.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                finish();
            }
        });
        iv4.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                finish();
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

            iv1.setImageBitmap(bmImg);
            iv2.setImageBitmap(bmImg);
            iv3.setImageBitmap(bmImg);
            iv4.setImageBitmap(bmImg);
        }

    }
}

