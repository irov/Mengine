package org.Mengine.Base;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Insets;
import android.graphics.Rect;
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
import android.view.WindowInsets;
import android.view.WindowManager;
import android.view.WindowMetrics;

import androidx.annotation.NonNull;

import java.util.concurrent.CountDownLatch;

public class MengineSurfaceView extends SurfaceView implements SurfaceHolder.Callback, View.OnKeyListener, View.OnTouchListener, SensorEventListener {
    public static final String TAG = "MengineSurfaceView";

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

        m_display = MengineUtils.getDefaultDisplay(context);

        m_sensorManager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);

        if (m_sensorManager != null) {
            m_accelerometer = m_sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            m_linearAccelerometer = m_sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
        }

        m_surfaceWidth = 1;
        m_surfaceHeight = 1;
    }

    public void handlePause() {
        this.setFocusable(false);
        this.setFocusableInTouchMode(false);
        this.setOnKeyListener(null);
        this.setOnTouchListener(null);

        if (m_sensorManager != null) {
            m_sensorManager.unregisterListener(this, m_accelerometer);
            m_sensorManager.unregisterListener(this, m_linearAccelerometer);
        }

        MengineActivity.AndroidPlatform_pauseEvent();
    }

    public void handleResume() {
        this.setFocusable(true);
        this.setFocusableInTouchMode(true);
        this.requestFocus();
        this.setOnKeyListener(this);
        this.setOnTouchListener(this);

        if (m_sensorManager != null) {
            m_sensorManager.registerListener(this, m_accelerometer, SensorManager.SENSOR_DELAY_GAME);
            m_sensorManager.registerListener(this, m_linearAccelerometer, SensorManager.SENSOR_DELAY_GAME);
        }

        MengineActivity.AndroidPlatform_resumeEvent();
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

        MengineActivity.AndroidPlatform_surfaceCreatedEvent(surface);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        MengineLog.logMessage(TAG, "surfaceDestroyed");

        Surface surface = holder.getSurface();

        if (surface == null) {
            MengineLog.logError(TAG, "surfaceDestroyed invalid surface");

            return;
        }

        MengineActivity.AndroidPlatform_surfaceDestroyedEvent(surface);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        MengineLog.logMessage(TAG, "surfaceChanged");

        m_surfaceWidth = width;
        m_surfaceHeight = height;

        int deviceWidth = width;
        int deviceHeight = height;

        try {
            Context context = this.getContext();

            Rect windowRect = MengineUtils.getDeviceWindowRect(context, m_display);

            deviceWidth = windowRect.width();
            deviceHeight = windowRect.height();
        } catch (final Exception e) {
            MengineLog.logMessage(TAG, "surfaceChanged invalid get device window rect");
        }

        float refreshRate = m_display.getRefreshRate();

        Surface surface = holder.getSurface();
        MengineActivity.AndroidPlatform_surfaceChangedEvent(surface, width, height, deviceWidth, deviceHeight, refreshRate);

        m_runLatch.countDown();
    }

    public static boolean isKeyEventHasText(KeyEvent event) {
        if (event.isCtrlPressed() == true) {
            return false;
        }

        if (event.isPrintingKey() == true) {
            return true;
        }

        if (event.getKeyCode() == KeyEvent.KEYCODE_SPACE) {
            return true;
        }

        return false;
    }

    public static boolean isSystemKey(int keyCode) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_BACK:
            case KeyEvent.KEYCODE_HOME:
            case KeyEvent.KEYCODE_MENU:
            case KeyEvent.KEYCODE_APP_SWITCH:
            case KeyEvent.KEYCODE_VOLUME_UP:
            case KeyEvent.KEYCODE_VOLUME_DOWN:
            case KeyEvent.KEYCODE_CAMERA:
            case KeyEvent.KEYCODE_POWER:
            case KeyEvent.KEYCODE_SEARCH:
                return true;
            default:
                return false;
        }
    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
        int action = event.getAction();
        int repeatCount = event.getRepeatCount();

        if (MengineSurfaceView.isSystemKey(keyCode) == true) {
            return false;
        }

        if (action == KeyEvent.ACTION_DOWN) {
            MengineActivity.AndroidPlatform_keyEvent(true, keyCode, repeatCount);

            if (MengineSurfaceView.isKeyEventHasText(event) == true) {
                int unicode = event.getUnicodeChar();

                MengineActivity.AndroidPlatform_textEvent(unicode);
            }

            return true;
        } else if (action == KeyEvent.ACTION_UP) {
            MengineActivity.AndroidPlatform_keyEvent(false, keyCode, repeatCount);

            return true;
        }

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

        MengineActivity.AndroidPlatform_touchEvent(action, pointerId, xn, yn, pn);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        int pointerCount = event.getPointerCount();
        int action = event.getActionMasked();

        switch (action) {
            case MotionEvent.ACTION_MOVE: {
                for (int index = 0; index != pointerCount; ++index) {
                    this.nativeTouchEvent(event, index, action);
                }
            }break;
            case MotionEvent.ACTION_UP: {
                this.nativeTouchEvent(event, 0, action);
                v.performClick();
            }break;
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

                MengineActivity.AndroidPlatform_accelerationEvent(xn, yn, zn);
            }break;
            case Sensor.TYPE_LINEAR_ACCELERATION: {
                //AndroidPlatform_accelerationEvent(xn, yn, zn);
            }break;
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        //ToDo
    }
}