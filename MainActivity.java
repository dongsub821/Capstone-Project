package com.example.caucse.db0602;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    ListView listview ;
    ListViewAdapter listAdapter;
    ArrayList<ListViewItem> listViewItemList;
    int imageName;
    Bitmap bmImg;
    ImageView imView;
    String imgUrl = "http://teamssdweb.kr/Cam_image/";
    String url = "http://teamssdweb.kr/DB/DB.php";
    back task;
    int number;
    // TextView
    TextView tv;
    ArrayList<String> Image_ID = new ArrayList<String>();
    ArrayList<String> Time = new ArrayList<String>();
    ArrayList<String> Time_detail = new ArrayList<String>();
    ArrayList<Integer> num = new ArrayList<Integer>();

    public GettingPHP gPHP;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        listview = (ListView) findViewById(R.id.listview1);
        listViewItemList = new ArrayList<ListViewItem>();
        gPHP = new GettingPHP();
        gPHP.execute(url);

        //  listViewItemList.add(new ListViewItem(ContextCompat.getDrawable(this, R.drawable.a2), "Image_2", "2018-04-08 11:17\n\nCAM2"));
        //   listViewItemList.add(new ListViewItem(ContextCompat.getDrawable(this, R.drawable.a3), "Image_3", "2018-04-08 09:55\n\nCAM1"));

        listview.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int position, long l) {
                Intent zoomintent = new Intent(MainActivity.this,SourceViewActivity.class);
                zoomintent.putExtra("imageName", position + 3);
                startActivity(zoomintent);
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

            listViewItemList.add(new ListViewItem( bmImg, Image_ID.get(number), Time.get(number)+Time_detail.get(number) +"\n\nCAM1"));
            listAdapter = new ListViewAdapter(MainActivity.this, listViewItemList);
            listview.setAdapter(listAdapter);
            number++;
        }

    }
    class GettingPHP extends AsyncTask<String, Integer, String> {

        @Override
        protected String doInBackground(String... params) {
            StringBuilder jsonHtml = new StringBuilder();
            try {
                URL phpUrl = new URL(params[0]);
                HttpURLConnection conn = (HttpURLConnection)phpUrl.openConnection();

                if ( conn != null ) {
                    conn.setConnectTimeout(10000);
                    conn.setUseCaches(false);

                    if ( conn.getResponseCode() == HttpURLConnection.HTTP_OK ) {
                        BufferedReader br = new BufferedReader(new InputStreamReader(conn.getInputStream(), "UTF-8"));
                        while ( true ) {
                            String line = br.readLine();
                            if ( line == null )
                                break;
                            jsonHtml.append(line + "\n");
                        }
                        br.close();
                    }
                    conn.disconnect();
                }
            } catch ( Exception e ) {
                e.printStackTrace();
            }
            return jsonHtml.toString();
        }

        protected void onPostExecute(String str) {
            try {
                JSONObject jObject = new JSONObject(str);
                JSONArray results = jObject.getJSONArray("result");
                for ( int i = 0; i < results.length(); ++i ) {
                    JSONObject temp = results.getJSONObject(i);
                    Image_ID.add(temp.get("Image_ID").toString());
                    Time.add(temp.get("Time").toString());
                    Time_detail.add(temp.get("Time_detail").toString());
                    num.add(i);

                    task = new back();
                    task.execute(imgUrl+Image_ID.get(num.get(i))+".png");
                }

            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    }
}
