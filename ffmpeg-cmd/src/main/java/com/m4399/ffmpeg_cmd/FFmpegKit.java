package com.m4399.ffmpeg_cmd;

import java.util.ArrayList;

/**
 * Author: lyman
 * Email: lymenye@gmail.com
 * Date: 2018/3/9
 * Description:
 */

public class FFmpegKit {


    private ArrayList<String> commands;


    static {
        System.loadLibrary("ffmpeg-cmd");
    }


    public FFmpegKit() {
        this.commands = new ArrayList<String>();
        this.commands.add("include/ffmpeg");
    }

    public native static int run(String[] commands);


}
