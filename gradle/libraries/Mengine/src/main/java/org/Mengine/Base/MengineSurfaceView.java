package org.Mengine.Base;

import android.content.Context;
import android.graphics.Canvas;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Build;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;

import androidx.annotation.NonNull;

import java.util.concurrent.CountDownLatch;

public class MengineSurfaceView extends SurfaceView implements SurfaceHolder.Callback, View.OnKeyListener, View.OnTouchListener, SensorEventListener {
    public static final String TAG = "MengineSurfaceView";

    private static native void AndroidPlatform_surfaceCreated(Surface surface);
    private static native void AndroidPlatform_surfaceDestroyed(Surface surface);
    private static native void AndroidPlatform_surfaceChanged(Surface surface, int surfaceWidth, int surfaceHeight, int deviceWidth, int deviceHeight, float rate);

    private static native void AndroidPlatform_touchEvent(int action, int pointerId, float x, float y, float pressure);
    private static native void AndroidPlatform_accelerationEvent(float x, float y, float z);

    protected CountDownLatch m_runLatch;

    private SensorManager m_sensorManager;
    private Sensor m_accelerometer;
    private Sensor m_linearAccelerometer;

    protected Display m_display;

    protected int m_surfaceWidth;
    protected int m_surfaceHeight;

    public MengineSurfaceView(Context context, CountDownLatch runLatch) {
        super(context);

        m_runLatch = runLatch;

        this.getHolder().addCallback(this);

        this.setFocusable(true);
        this.setFocusableInTouchMode(true);
        this.requestFocus();
        this.setOnKeyListener(this);
        this.setOnTouchListener(this);

        WindowManager windowManager = (WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
        m_display = windowManager.getDefaultDisplay();

        m_sensorManager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);

        if (m_sensorManager != null) {
            m_accelerometer = m_sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            m_linearAccelerometer = m_sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
        }

        m_surfaceWidth = 1;
        m_surfaceHeight = 1;
    }

    public MengineSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void handlePause() {
        if (m_sensorManager != null) {
            m_sensorManager.unregisterListener(this, m_accelerometer);
            m_sensorManager.unregisterListener(this, m_linearAccelerometer);
        }
    }

    public void handleResume() {
        if (m_sensorManager != null) {
            m_sensorManager.registerListener(this, m_accelerometer, SensorManager.SENSOR_DELAY_GAME);
            m_sensorManager.registerListener(this, m_linearAccelerometer, SensorManager.SENSOR_DELAY_GAME);
        }
    }

    Surface getSurface() {
        return this.getHolder().getSurface();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        MengineLog.logMessage(TAG, "surfaceCreated");

        Surface surface = holder.getSurface();

        if (surface == null) {
            MengineLog.logError(TAG, "surfaceCreated invalid surface");

            return;
        }

        AndroidPlatform_surfaceCreated(surface);

        //SDLActivity.onNativeSurfaceCreated();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        MengineLog.logMessage(TAG, "surfaceDestroyed");

        Surface surface = holder.getSurface();

        if (surface == null) {
            MengineLog.logError(TAG, "surfaceDestroyed invalid surface");

            return;
        }

        AndroidPlatform_surfaceDestroyed(surface);

        //mIsSurfaceReady = false;
        //SDLActivity.onNativeSurfaceDestroyed();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        MengineLog.logMessage(TAG, "surfaceChanged");

        m_surfaceWidth = width;
        m_surfaceHeight = height;

        int deviceWidth = width;
        int deviceHeight = height;
        try {
            DisplayMetrics realMetrics = new DisplayMetrics();
            m_display.getRealMetrics( realMetrics );

            deviceWidth = realMetrics.widthPixels;
            deviceHeight = realMetrics.heightPixels;
        } catch(Exception e) {
            //Ignore
        }

        float refreshRate = m_display.getRefreshRate();

        Surface surface = holder.getSurface();
        AndroidPlatform_surfaceChanged(surface, width, height, deviceWidth, deviceHeight, refreshRate);

        m_runLatch.countDown();
    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
        MengineLog.logMessage(TAG, "onKey");

        return false;
    }

    protected void nativeTouchEvent(MotionEvent event, int index, int action) {
        int pointerId = event.getPointerId(index);
        float x = event.getX(index);
        float y = event.getY(index);
        float p = event.getPressure(index);

        float xn = x / m_surfaceWidth;
        float yn = y / m_surfaceHeight;
        float pn = p > 1.f ? 1.f : p;

        AndroidPlatform_touchEvent(action, pointerId, xn, yn, pn);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        MengineLog.logMessage(TAG, "onTouch");

        int pointerCount = event.getPointerCount();
        int action = event.getActionMasked();

        switch (action) {
            case MotionEvent.ACTION_MOVE: {
                for (int index = 0; index != pointerCount; ++index) {
                    this.nativeTouchEvent(event, index, action);
                }
            }break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_DOWN: {
                this.nativeTouchEvent(event, 0, action);
            }break;
            case MotionEvent.ACTION_POINTER_UP:
            case MotionEvent.ACTION_POINTER_DOWN: {
                int index = event.getActionIndex();

                this.nativeTouchEvent(event, index, action);
            }break;
            case MotionEvent.ACTION_CANCEL: {
                for (int index = 0; index != pointerCount; ++index) {
                    this.nativeTouchEvent(event, index, MotionEvent.ACTION_UP);
                }
            }break;
        }

        return true;
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        int type = event.sensor.getType();

        switch (type) {
            case Sensor.TYPE_ACCELEROMETER: {
                float x = event.values[0];
                float y = event.values[1];
                float z = event.values[2];

                float xn = x / SensorManager.GRAVITY_EARTH;
                float yn = y / SensorManager.GRAVITY_EARTH;
                float zn = z / SensorManager.GRAVITY_EARTH;

                AndroidPlatform_accelerationEvent(xn, yn, zn);
            }break;
            case Sensor.TYPE_LINEAR_ACCELERATION: {
                //AndroidPlatform_accelerationEvent(xn, yn, zn);
            }break;
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        //ToDo: Handle accuracy changes here if needed
    }
}