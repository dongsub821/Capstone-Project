package com.example.caucse.db;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;

public class ListViewAdapter extends BaseAdapter {

    ImageView iconImageView;
    TextView titleTextView;
    TextView descTextView;

    Context context;
    ArrayList<ListViewItem> listViewItemList = new ArrayList<ListViewItem>() ;

    public ListViewAdapter(Context context, ArrayList<ListViewItem> listViewItemList) {
        this.context = context;
        this.listViewItemList = listViewItemList;
    }

    @Override
    public int getCount() {
        return this.listViewItemList.size();
    }

    @Override
    public Object getItem(int position) {
        return listViewItemList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        if (convertView == null) {

            LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = inflater.inflate(R.layout.listview_item, parent, false);

            iconImageView = (ImageView) convertView.findViewById(R.id.imageView1);
            titleTextView = (TextView) convertView.findViewById(R.id.textView1);
            descTextView = (TextView) convertView.findViewById(R.id.textView2);

            ListViewItem listViewItem = listViewItemList.get(position);

            iconImageView.setImageBitmap(listViewItem.getIcon());
            titleTextView.setText(listViewItem.getTitle());
            descTextView.setText(listViewItem.getDesc());

        }

        return convertView;
    }

    /*
    // 아이템 데이터 추가를 위한 함수. 개발자가 원하는대로 작성 가능.
    public void addItem(Drawable icon, String title, String desc) {
        ListViewItem item = new ListViewItem();

        item.setIcon(icon);
        item.setTitle(title);
        item.setDesc(desc);

        listViewItemList.add(item);
    }*/
}