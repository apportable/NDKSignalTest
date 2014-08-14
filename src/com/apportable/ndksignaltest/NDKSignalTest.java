package com.apportable.ndksignaltest;

import android.app.Activity;
import android.os.Bundle;
import android.view.ViewGroup;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
import android.widget.AbsoluteLayout;
import android.widget.EditText;
import android.widget.FrameLayout;

import com.apportable.ndksignaltest.BGThread;

public class NDKSignalTest extends Activity {

    static {
        System.loadLibrary("NDKSignalTest");
    }   

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        new BGThread("java1").start();
        new BGThread("java2").start();
        new BGThread("java3").start();
        new BGThread("java4").start();
        new BGThread("java5").start();
        new BGThread("java6").start();
        beginTest();
    }

    @Override
    protected void onResume() {
      super.onResume();
    }

    @Override
    protected void onPause() {
      super.onPause();
    }

    private static native void beginTest();
}
