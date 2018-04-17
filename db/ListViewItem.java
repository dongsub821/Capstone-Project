package com.example.caucse.db;

import android.graphics.Bitmap;

/**
 * Created by caucse on 2018-04-10.
 */

public class ListViewItem {
    private Bitmap iconBitmap ;
    private String titleStr ;
    private String descStr ;

    public ListViewItem(Bitmap iconBitmap, String titleStr, String descStr) {
        this.iconBitmap = iconBitmap;
        this.titleStr = titleStr;
        this.descStr = descStr;
    }


    public void setIcon(Bitmap icon) {
        iconBitmap = icon ;
    }

    public void setTitle(String title) {

        titleStr = title ;
    }
    public void setDesc(String desc) {
        descStr = desc ;
    }

    public Bitmap getIcon() {
        return this.iconBitmap ;
    }
    public String getTitle() {
        return this.titleStr ;
    }
    public String getDesc() {
        return this.descStr ;
    }
}