package org.Mengine.Base;

import android.content.Context;
import android.graphics.Rect;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import androidx.annotation.NonNull;

public class MengineSurfaceView extends SurfaceView implements SurfaceHolder.Callback, View.OnKeyListener, View.OnTouchListener, SensorEventListener {
    public static final MengineTag TAG = MengineTag.of("MNGSurfaceView");

    private SensorManager m_sensorManager;
    private Sensor m_accelerometer;
    private Sensor m_linearAccelerometer;

    private Display m_display;

    private float m_surfaceWidthF;
    private float m_surfaceHeightF;

    private boolean m_paused;

    public MengineSurfaceView(Context context) {
        super(context);

        this.getHolder().addCallback(this);

        m_display = MengineUtils.getDefaultDisplay(context);

        if (BuildConfig.ANDROID_APP_REQUIRED_HARDWARE_SENSOR_ACCELEROMETER == true) {
            m_sensorManager = context.getSystemService(SensorManager.class);

            if (m_sensorManager != null) {
                m_accelerometer = m_sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
                m_linearAccelerometer = m_sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
            }
        }

        m_surfaceWidthF = 1.f;
        m_surfaceHeightF = 1.f;
        m_paused = true;
    }

    public void handleStart() {
        if (m_paused == false) {
            return;
        }

        MengineLog.logInfo(TAG, "handleStart");

        this.handleResume();
    }

    public void handleStop() {
        if (m_paused == true) {
            return;
        }

        MengineLog.logInfo(TAG, "handleStop");

        this.handlePause();
    }

    public void handleResume() {
        if (m_paused == false) {
            return;
        }

        MengineLog.logInfo(TAG, "handleResume");

        m_paused = false;

        this.setFocusable(true);
        this.setFocusableInTouchMode(true);
        this.requestFocus();
        this.setOnKeyListener(this);
        this.setOnTouchListener(this);

        if (m_sensorManager != null) {
            if (m_accelerometer != null) {
                m_sensorManager.registerListener(this, m_accelerometer, SensorManager.SENSOR_DELAY_GAME);
            }

            if (m_linearAccelerometer != null) {
                m_sensorManager.registerListener(this, m_linearAccelerometer, SensorManager.SENSOR_DELAY_GAME);
            }
        }

        float x = MengineNative.AndroidPlatform_getLastFingerX();
        float y = MengineNative.AndroidPlatform_getLastFingerY();
        MenginePlatformEventQueue.pushResumeEvent(x, y);
    }

    public void handlePause() {
        if (m_paused == true) {
            return;
        }

        MengineLog.logDebug(TAG, "handlePause");

        m_paused = true;

        this.setFocusable(false);
        this.setFocusableInTouchMode(false);
        this.setOnKeyListener(null);
        this.setOnTouchListener(null);

        if (m_sensorManager != null) {
            if (m_accelerometer != null) {
                m_sensorManager.unregisterListener(this, m_accelerometer);
            }

            if (m_linearAccelerometer != null) {
                m_sensorManager.unregisterListener(this, m_linearAccelerometer);
            }
        }

        float x = MengineNative.AndroidPlatform_getLastFingerX();
        float y = MengineNative.AndroidPlatform_getLastFingerY();
        MenginePlatformEventQueue.pushPauseEvent(x, y);
    }

    public void handleDestroy() {
        MengineLog.logInfo(TAG, "handleDestroy");

        this.handleStop();

        this.getHolder().removeCallback(this);

        m_display = null;
        m_sensorManager = null;
        m_accelerometer = null;
        m_linearAccelerometer = null;
    }

    Surface getSurface() {
        return this.getHolder().getSurface();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        MengineLog.logInfo(TAG, "surfaceCreated");

        Surface surface = holder.getSurface();

        if (surface == null) {
            MengineLog.logError(TAG, "surfaceCreated invalid surface");

            return;
        }

        MenginePlatformEventQueue.pushSurfaceCreatedEvent(surface);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        MengineLog.logInfo(TAG, "surfaceDestroyed");

        MenginePlatformEventQueue.pushSurfaceDestroyedEvent();
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        MengineLog.logInfo(TAG, "surfaceChanged format: %d width: %d height: %d"
            , format
            , width
            , height
        );

        m_surfaceWidthF = width > 0 ? (float)width : 1.f;
        m_surfaceHeightF = height > 0 ? (float)height : 1.f;

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

        MenginePlatformEventQueue.pushSurfaceChangedEvent(surface, width, height, deviceWidth, deviceHeight, refreshRate);

        MengineMain.runLatch();
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

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
        if (event.isSystem() == true) {
            return false;
        }

        int action = event.getAction();
        int repeatCount = event.getRepeatCount();

        if (action == KeyEvent.ACTION_DOWN) {
            MengineNative.AndroidPlatform_keyEvent(true, keyCode, repeatCount);

            if (MengineSurfaceView.isKeyEventHasText(event) == true) {
                int unicode = event.getUnicodeChar();

                MengineNative.AndroidPlatform_textEvent(unicode);
            }

            return true;
        } else if (action == KeyEvent.ACTION_UP) {
            MengineNative.AndroidPlatform_keyEvent(false, keyCode, repeatCount);

            return true;
        }

        return false;
    }

    protected void nativeTouchEvent(MotionEvent event, int index, int action) {
        int pointerId = event.getPointerId(index);
        float x = event.getX(index);
        float y = event.getY(index);
        float p = event.getPressure(index);

        this.nativeTouchEvent(pointerId, x, y, p, action);
    }

    protected void nativeTouchEvent(int pointerId, float x, float y, float p, int action) {
        float xn = x / m_surfaceWidthF;
        float yn = y / m_surfaceHeightF;
        float pn = Math.min(p, 1.f);

        MengineNative.AndroidPlatform_touchEvent(action, pointerId, xn, yn, pn);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        int pointerCount = event.getPointerCount();
        int action = event.getActionMasked();

        switch (action) {
            case MotionEvent.ACTION_MOVE: {
                final int historySize = event.getHistorySize();

                for (int historyIndex = 0; historyIndex != historySize; ++historyIndex) {
                    for (int pointerIndex = 0; pointerIndex != pointerCount; ++pointerIndex) {
                        int pointerId = event.getPointerId(pointerIndex);
                        float x  = event.getHistoricalX(pointerIndex, historyIndex);
                        float y  = event.getHistoricalY(pointerIndex, historyIndex);
                        float p = event.getHistoricalPressure(pointerIndex, historyIndex);

                        this.nativeTouchEvent(pointerId, x, y, p, MotionEvent.ACTION_MOVE);
                    }
                }

                for (int pointerIndex = 0; pointerIndex != pointerCount; ++pointerIndex) {
                    this.nativeTouchEvent(event, pointerIndex, action);
                }
            }break;
            case MotionEvent.ACTION_UP: {
                int idx = event.getActionIndex();
                this.nativeTouchEvent(event, idx, action);
                v.performClick();
            }break;
            case MotionEvent.ACTION_DOWN: {
                int idx = event.getActionIndex();
                this.nativeTouchEvent(event, idx, action);
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
    public boolean performClick() {
        super.performClick();

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

                MengineNative.AndroidPlatform_accelerationEvent(xn, yn, zn);
            }break;
            case Sensor.TYPE_LINEAR_ACCELERATION: {
                //AndroidPlatform_accelerationEvent(xn, yn, zn);
            }break;
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        MengineLog.logMessage(TAG, "onAccuracyChanged sensor: %s accuracy: %d"
            , sensor.getName()
            , accuracy
        );

        //ToDo
    }
}