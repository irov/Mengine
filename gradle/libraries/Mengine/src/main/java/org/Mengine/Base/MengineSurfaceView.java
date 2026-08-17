package org.Mengine.Base;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Insets;
import android.graphics.Rect;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.hardware.input.InputManager;
import android.os.Build;
import android.os.SystemClock;
import android.util.SparseBooleanArray;
import android.util.SparseIntArray;
import android.view.Display;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowInsets;

import androidx.annotation.NonNull;

public class MengineSurfaceView extends SurfaceView implements SurfaceHolder.Callback, View.OnKeyListener, View.OnTouchListener, SensorEventListener, InputManager.InputDeviceListener {
    public static final MengineTag TAG = MengineTag.of("MNGSurfaceView");

    private SensorManager m_sensorManager;
    private InputManager m_inputManager;
    private Sensor m_accelerometer;
    private Sensor m_linearAccelerometer;

    private Display m_display;

    private float m_surfaceWidthF;
    private float m_surfaceHeightF;

    private boolean m_paused;
    private final SparseBooleanArray m_controllerDevices = new SparseBooleanArray();
    private final SparseIntArray m_controllerHatStates = new SparseIntArray();

    public MengineSurfaceView(Context context) {
        super(context);

        this.getHolder().addCallback(this);

        m_display = MengineUtils.getDefaultDisplay(context);
        m_inputManager = (InputManager)context.getSystemService(Context.INPUT_SERVICE);

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

        this.setOnApplyWindowInsetsListener(this::onApplyWindowInsets);
    }

    private WindowInsets onApplyWindowInsets(View view, WindowInsets windowInsets) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            this.dispatchSafeAreaViewportR(view, windowInsets);
        } else {
            this.dispatchSafeAreaViewportLegacy(view, windowInsets);
        }

        return windowInsets;
    }

    @TargetApi(Build.VERSION_CODES.R)
    private void dispatchSafeAreaViewportR(View view, WindowInsets windowInsets) {
        Insets insets = windowInsets.getInsets(WindowInsets.Type.systemBars() | WindowInsets.Type.displayCutout());
        this.dispatchSafeAreaViewport(view, insets.left, insets.top, insets.right, insets.bottom);
    }

    @SuppressWarnings("deprecation")
    private void dispatchSafeAreaViewportLegacy(View view, WindowInsets windowInsets) {
        int left = windowInsets.getSystemWindowInsetLeft();
        int top = windowInsets.getSystemWindowInsetTop();
        int right = windowInsets.getSystemWindowInsetRight();
        int bottom = windowInsets.getSystemWindowInsetBottom();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P && windowInsets.getDisplayCutout() != null) {
            left = Math.max(left, windowInsets.getDisplayCutout().getSafeInsetLeft());
            top = Math.max(top, windowInsets.getDisplayCutout().getSafeInsetTop());
            right = Math.max(right, windowInsets.getDisplayCutout().getSafeInsetRight());
            bottom = Math.max(bottom, windowInsets.getDisplayCutout().getSafeInsetBottom());
        }

        this.dispatchSafeAreaViewport(view, left, top, right, bottom);
    }

    private void dispatchSafeAreaViewport(View view, int left, int top, int right, int bottom) {
        int width = view.getWidth();
        int height = view.getHeight();

        if (width != 0 && height != 0) {
            MengineNative.AndroidPlatform_safeAreaViewportEvent(
                (float)left,
                (float)top,
                (float)Math.max(left, width - right),
                (float)Math.max(top, height - bottom)
            );
        }
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

        if (m_inputManager != null) {
            m_inputManager.registerInputDeviceListener(this, null);

            for (int deviceId : m_inputManager.getInputDeviceIds()) {
                this.updateControllerDevice(deviceId);
            }
        }

        this.requestApplyInsets();
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

        if (m_inputManager != null) {
            m_inputManager.unregisterInputDeviceListener(this);
        }

        for (int index = 0; index != m_controllerDevices.size(); ++index) {
            int deviceId = m_controllerDevices.keyAt(index);
            MengineNative.AndroidPlatform_controllerConnectEvent(SystemClock.uptimeMillis(), deviceId + 1, false);
        }

        m_controllerDevices.clear();
        m_controllerHatStates.clear();
    }

    public void handleDestroy() {
        MengineLog.logInfo(TAG, "handleDestroy");

        this.handleStop();

        this.getHolder().removeCallback(this);

        m_display = null;
        m_inputManager = null;
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
        long eventTime = event.getEventTime();

        if (isControllerSource(event.getSource()) == true) {
            boolean isDown = action == KeyEvent.ACTION_DOWN;

            if (action == KeyEvent.ACTION_DOWN || action == KeyEvent.ACTION_UP) {
                MengineNative.AndroidPlatform_controllerButtonEvent(eventTime, event.getDeviceId() + 1, keyCode, isDown == true ? 1.f : 0.f, isDown);
                return true;
            }
        }

        if (action == KeyEvent.ACTION_DOWN) {
            MengineNative.AndroidPlatform_keyEvent(eventTime, true, keyCode, repeatCount);

            if (MengineSurfaceView.isKeyEventHasText(event) == true) {
                int unicode = event.getUnicodeChar();

                MengineNative.AndroidPlatform_textEvent(eventTime, unicode);
            }

            return true;
        } else if (action == KeyEvent.ACTION_UP) {
            MengineNative.AndroidPlatform_keyEvent(eventTime, false, keyCode, repeatCount);

            return true;
        }

        return false;
    }

    private static boolean isControllerSource(int source) {
        return (source & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD
            || (source & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK;
    }

    private static boolean isControllerDevice(InputDevice device) {
        return device != null && isControllerSource(device.getSources()) == true;
    }

    private void updateControllerDevice(int deviceId) {
        InputDevice device = InputDevice.getDevice(deviceId);
        boolean controller = isControllerDevice(device);
        boolean registered = m_controllerDevices.get(deviceId, false);

        if (controller == registered) {
            return;
        }

        if (controller == true) {
            m_controllerDevices.put(deviceId, true);
        } else {
            m_controllerDevices.delete(deviceId);
        }

        MengineNative.AndroidPlatform_controllerConnectEvent(SystemClock.uptimeMillis(), deviceId + 1, controller);
    }

    @Override
    public void onInputDeviceAdded(int deviceId) {
        this.updateControllerDevice(deviceId);
    }

    @Override
    public void onInputDeviceRemoved(int deviceId) {
        if (m_controllerDevices.get(deviceId, false) == false) {
            return;
        }

        m_controllerDevices.delete(deviceId);
        m_controllerHatStates.delete(deviceId);
        MengineNative.AndroidPlatform_controllerConnectEvent(SystemClock.uptimeMillis(), deviceId + 1, false);
    }

    @Override
    public void onInputDeviceChanged(int deviceId) {
        this.updateControllerDevice(deviceId);
    }

    @Override
    public boolean onGenericMotionEvent(MotionEvent event) {
        if (event.getAction() != MotionEvent.ACTION_MOVE || isControllerSource(event.getSource()) == false) {
            return super.onGenericMotionEvent(event);
        }

        int deviceId = event.getDeviceId() + 1;
        long eventTime = event.getEventTime();
        InputDevice inputDevice = event.getDevice();
        int rightXAxis = inputDevice != null && inputDevice.getMotionRange(MotionEvent.AXIS_Z, event.getSource()) != null
            ? MotionEvent.AXIS_Z
            : MotionEvent.AXIS_RX;
        int rightYAxis = inputDevice != null && inputDevice.getMotionRange(MotionEvent.AXIS_RZ, event.getSource()) != null
            ? MotionEvent.AXIS_RZ
            : MotionEvent.AXIS_RY;
        int leftTriggerAxis = inputDevice != null && inputDevice.getMotionRange(MotionEvent.AXIS_LTRIGGER, event.getSource()) != null
            ? MotionEvent.AXIS_LTRIGGER
            : MotionEvent.AXIS_BRAKE;
        int rightTriggerAxis = inputDevice != null && inputDevice.getMotionRange(MotionEvent.AXIS_RTRIGGER, event.getSource()) != null
            ? MotionEvent.AXIS_RTRIGGER
            : MotionEvent.AXIS_GAS;

        int[] axes = {MotionEvent.AXIS_X, MotionEvent.AXIS_Y, rightXAxis, rightYAxis, leftTriggerAxis, rightTriggerAxis};

        for (int axis : axes) {
            MengineNative.AndroidPlatform_controllerAxisEvent(eventTime, deviceId, axis, event.getAxisValue(axis));
        }

        this.dispatchControllerHat(eventTime, event.getDeviceId(), event.getAxisValue(MotionEvent.AXIS_HAT_X), event.getAxisValue(MotionEvent.AXIS_HAT_Y));

        return true;
    }

    private void dispatchControllerHat(long eventTime, int rawDeviceId, float x, float y) {
        final int LEFT = 1;
        final int RIGHT = 2;
        final int UP = 4;
        final int DOWN = 8;
        int state = (x < -0.5f ? LEFT : x > 0.5f ? RIGHT : 0) | (y < -0.5f ? UP : y > 0.5f ? DOWN : 0);
        int previous = m_controllerHatStates.get(rawDeviceId, 0);

        if (state == previous) {
            return;
        }

        int[] masks = {LEFT, RIGHT, UP, DOWN};
        int[] keyCodes = {KeyEvent.KEYCODE_DPAD_LEFT, KeyEvent.KEYCODE_DPAD_RIGHT, KeyEvent.KEYCODE_DPAD_UP, KeyEvent.KEYCODE_DPAD_DOWN};

        for (int index = 0; index != masks.length; ++index) {
            boolean wasDown = (previous & masks[index]) != 0;
            boolean isDown = (state & masks[index]) != 0;

            if (wasDown != isDown) {
                MengineNative.AndroidPlatform_controllerButtonEvent(eventTime, rawDeviceId + 1, keyCodes[index], isDown ? 1.f : 0.f, isDown);
            }
        }

        m_controllerHatStates.put(rawDeviceId, state);
    }

    protected void nativeTouchEvent(MotionEvent event, int index, int action) {
        long eventTime = event.getEventTime();

        this.nativeTouchEvent(eventTime, event, index, action);
    }

    protected void nativeTouchEvent(long eventTime, MotionEvent event, int index, int action) {
        int pointerId = event.getPointerId(index);
        float x = event.getX(index);
        float y = event.getY(index);
        float p = event.getPressure(index);

        this.nativeTouchEvent(eventTime, pointerId, x, y, p, action);
    }

    protected void nativeTouchEvent(long eventTime, int pointerId, float x, float y, float p, int action) {
        float xn = x / m_surfaceWidthF;
        float yn = y / m_surfaceHeightF;
        float pn = Math.min(p, 1.f);

        MengineNative.AndroidPlatform_touchEvent(eventTime, action, pointerId, xn, yn, pn);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        int pointerCount = event.getPointerCount();
        int action = event.getActionMasked();

        switch (action) {
            case MotionEvent.ACTION_MOVE: {
                final int historySize = event.getHistorySize();

                for (int historyIndex = 0; historyIndex != historySize; ++historyIndex) {
                    long historicalEventTime = event.getHistoricalEventTime(historyIndex);

                    for (int pointerIndex = 0; pointerIndex != pointerCount; ++pointerIndex) {
                        int pointerId = event.getPointerId(pointerIndex);
                        float x  = event.getHistoricalX(pointerIndex, historyIndex);
                        float y  = event.getHistoricalY(pointerIndex, historyIndex);
                        float p = event.getHistoricalPressure(pointerIndex, historyIndex);

                        this.nativeTouchEvent(historicalEventTime, pointerId, x, y, p, MotionEvent.ACTION_MOVE);
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
                long eventTime = event.timestamp;

                float x = event.values[0];
                float y = event.values[1];
                float z = event.values[2];

                float xn = x / SensorManager.GRAVITY_EARTH;
                float yn = y / SensorManager.GRAVITY_EARTH;
                float zn = z / SensorManager.GRAVITY_EARTH;

                MengineNative.AndroidPlatform_accelerationEvent(eventTime, xn, yn, zn);
            }break;
            case Sensor.TYPE_LINEAR_ACCELERATION: {
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
