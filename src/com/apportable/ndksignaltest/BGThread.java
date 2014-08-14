package com.apportable.ndksignaltest;

import java.util.Random;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

@SuppressWarnings("deprecation")
public class BGThread extends Thread {

    private static long LOGGING_THRESHOLD = 2000;

    private static void log(String message, Object ... arguments) {
        Log.d("BGThread", String.format(message, arguments));
    }

    private String mThreadId;

    public BGThread(String threadId) {
        super(Thread.currentThread().getThreadGroup(), null, "BGThread", 1024 * 1024 * 1024 * 5); // ???
        mThreadId = threadId;
    }

    @Override
    public void run() {
        long prev = System.currentTimeMillis();
        while (true) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                log(mThreadId+" interrupted");
            }
            Object obj = new Object();
            long now = System.currentTimeMillis();
            if (now-prev > LOGGING_THRESHOLD) {
                prev = now;
                log(mThreadId+" ... random object hash : "+obj.hashCode());
            }
        }
    }
}
