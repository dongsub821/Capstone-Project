package com.example.caucse.db;

import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class MainActivity extends AppCompatActivity {
    // 데이터를 받아올 PHP 주소
    String url = "http://teamssdweb.kr/DB/DB.php";
    // 데이터를 보기위한 TextView
    TextView tv;
    // PHP를 읽어올때 사용할 변수
    public GettingPHP gPHP;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        gPHP = new GettingPHP();

        tv = (TextView)findViewById(R.id.textView);

        gPHP.execute(url);
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
                String txt = "";
                for ( int i = 0; i < results.length(); ++i ) {
                    JSONObject temp = results.getJSONObject(i);
                    txt +=temp.get("Image_ID");
                    txt +=" ";
                    txt +=temp.get("Time");
                    txt +=" ";
                    txt +=temp.get("Time_detail");
                    txt +="\n";


                }
                tv.setText(txt);
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    }
}


