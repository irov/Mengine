package org.libsdl.app;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.Hashtable;
import java.lang.reflect.Method;

import android.app.*;
import android.content.*;
import android.content.res.Configuration;
import android.text.InputType;
import android.view.*;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;
import android.widget.RelativeLayout;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.os.*;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.SparseArray;
import android.graphics.*;
import android.graphics.drawable.Drawable;
import android.hardware.*;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;

/**
    SDLSurface. This is what we draw on, so we need to know when it's created
    in order to do anything useful.

    Because of this, that's where we set up the SDL thread
*/
public class SDLSurface extends SurfaceView implements SurfaceHolder.Callback,
    View.OnKeyListener, View.OnTouchListener, SensorEventListener  {

    // Sensors
    protected static SensorManager mSensorManager;
    protected static Display mDisplay;

    // Keep track of the surface size to normalize touch events
    protected static float mWidth, mHeight;

    // Startup
    public SDLSurface(Context context) {
        super(context);
        getHolder().addCallback(this);

        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
        setOnKeyListener(this);
        setOnTouchListener(this);

        mDisplay = ((WindowManager)context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        mSensorManager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);

        if (Build.VERSION.SDK_INT >= 12) {
            setOnGenericMotionListener(SDLActivity.getMotionListener());
        }

        // Some arbitrary defaults to avoid a potential division by zero
        mWidth = 1.0f;
        mHeight = 1.0f;
    }

    public void handlePause() {
        enableSensor(Sensor.TYPE_ACCELEROMETER, false);
    }

    public void handleResume() {
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
        setOnKeyListener(this);
        setOnTouchListener(this);
        enableSensor(Sensor.TYPE_ACCELEROMETER, true);
    }

    public Surface getNativeSurface() {
        return getHolder().getSurface();
    }

    // Called when we have a valid drawing surface
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.v("SDL", "surfaceCreated()");
        holder.setType(SurfaceHolder.SURFACE_TYPE_GPU);
    }

    // Called when we lose the surface
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.v("SDL", "surfaceDestroyed()");

        // Transition to pause, if needed
        SDLActivity.mNextNativeState = SDLActivity.NativeState.PAUSED;
        SDLActivity.handleNativeState();

        SDLActivity.mIsSurfaceReady = false;
        SDLActivity.onNativeSurfaceDestroyed();
    }

    // Called when the surface is resized
    @Override
    public void surfaceChanged(SurfaceHolder holder,
                               int format, int width, int height) {
        Log.v("SDL", "surfaceChanged()");

        if (SDLActivity.mSingleton == null) {
            return;
        }

        int sdlFormat = 0x15151002; // SDL_PIXELFORMAT_RGB565 by default
        switch (format) {
        case PixelFormat.A_8:
            Log.v("SDL", "pixel format A_8");
            break;
        case PixelFormat.LA_88:
            Log.v("SDL", "pixel format LA_88");
            break;
        case PixelFormat.L_8:
            Log.v("SDL", "pixel format L_8");
            break;
        case PixelFormat.RGBA_4444:
            Log.v("SDL", "pixel format RGBA_4444");
            sdlFormat = 0x15421002; // SDL_PIXELFORMAT_RGBA4444
            break;
        case PixelFormat.RGBA_5551:
            Log.v("SDL", "pixel format RGBA_5551");
            sdlFormat = 0x15441002; // SDL_PIXELFORMAT_RGBA5551
            break;
        case PixelFormat.RGBA_8888:
            Log.v("SDL", "pixel format RGBA_8888");
            sdlFormat = 0x16462004; // SDL_PIXELFORMAT_RGBA8888
            break;
        case PixelFormat.RGBX_8888:
            Log.v("SDL", "pixel format RGBX_8888");
            sdlFormat = 0x16261804; // SDL_PIXELFORMAT_RGBX8888
            break;
        case PixelFormat.RGB_332:
            Log.v("SDL", "pixel format RGB_332");
            sdlFormat = 0x14110801; // SDL_PIXELFORMAT_RGB332
            break;
        case PixelFormat.RGB_565:
            Log.v("SDL", "pixel format RGB_565");
            sdlFormat = 0x15151002; // SDL_PIXELFORMAT_RGB565
            break;
        case PixelFormat.RGB_888:
            Log.v("SDL", "pixel format RGB_888");
            // Not sure this is right, maybe SDL_PIXELFORMAT_RGB24 instead?
            sdlFormat = 0x16161804; // SDL_PIXELFORMAT_RGB888
            break;
        default:
            Log.v("SDL", "pixel format unknown " + format);
            break;
        }

        mWidth = width;
        mHeight = height;
        int nDeviceWidth = width;
        int nDeviceHeight = height;
        try
        {
            if (Build.VERSION.SDK_INT >= 17) {
                android.util.DisplayMetrics realMetrics = new android.util.DisplayMetrics();
                mDisplay.getRealMetrics( realMetrics );
                nDeviceWidth = realMetrics.widthPixels;
                nDeviceHeight = realMetrics.heightPixels;
            }
        }
        catch ( java.lang.Throwable throwable ) {}

        synchronized(SDLActivity.getContext()) {
            // In case we're waiting on a size change after going fullscreen, send a notification.
            SDLActivity.getContext().notifyAll();
        }

        Log.v("SDL", "Window size: " + width + "x" + height);
        Log.v("SDL", "Device size: " + nDeviceWidth + "x" + nDeviceHeight);
        SDLActivity.onNativeResize(width, height, nDeviceWidth, nDeviceHeight, sdlFormat, mDisplay.getRefreshRate());

        boolean skip = false;
        int requestedOrientation = SDLActivity.mSingleton.getRequestedOrientation();

        if (requestedOrientation == ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED)
        {
            // Accept any
        }
        else if (requestedOrientation == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT || requestedOrientation == ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT)
        {
            if (mWidth > mHeight) {
               skip = true;
            }
        } else if (requestedOrientation == ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE || requestedOrientation == ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE) {
            if (mWidth < mHeight) {
               skip = true;
            }
        }

        // Special Patch for Square Resolution: Black Berry Passport
        if (skip) {
           double min = Math.min(mWidth, mHeight);
           double max = Math.max(mWidth, mHeight);

           if (max / min < 1.20) {
              Log.v("SDL", "Don't skip on such aspect-ratio. Could be a square resolution.");
              skip = false;
           }
        }

        if (skip) {
           Log.v("SDL", "Skip .. Surface is not ready.");
           SDLActivity.mIsSurfaceReady = false;
           return;
        }

        /* Surface is ready */
        SDLActivity.mIsSurfaceReady = true;

        /* If the surface has been previously destroyed by onNativeSurfaceDestroyed, recreate it here */
        SDLActivity.onNativeSurfaceChanged();

        SDLActivity.handleNativeState();
    }

    // Key events
    @Override
    public boolean onKey(View  v, int keyCode, KeyEvent event) {
        // Dispatch the different events depending on where they come from
        // Some SOURCE_JOYSTICK, SOURCE_DPAD or SOURCE_GAMEPAD are also SOURCE_KEYBOARD
        // So, we try to process them as JOYSTICK/DPAD/GAMEPAD events first, if that fails we try them as KEYBOARD
        //
        // Furthermore, it's possible a game controller has SOURCE_KEYBOARD and
        // SOURCE_JOYSTICK, while its key events arrive from the keyboard source
        // So, retrieve the device itself and check all of its sources
        if (SDLControllerManager.isDeviceSDLJoystick(event.getDeviceId())) {
            // Note that we process events with specific key codes here
            if (event.getAction() == KeyEvent.ACTION_DOWN) {
                if (SDLControllerManager.onNativePadDown(event.getDeviceId(), keyCode) == 0) {
                    return true;
                }
            } else if (event.getAction() == KeyEvent.ACTION_UP) {
                if (SDLControllerManager.onNativePadUp(event.getDeviceId(), keyCode) == 0) {
                    return true;
                }
            }
        }

        if ((event.getSource() & InputDevice.SOURCE_KEYBOARD) != 0) {
            if (event.getAction() == KeyEvent.ACTION_DOWN) {
                //Log.v("SDL", "key down: " + keyCode);
                if (SDLActivity.isTextInputEvent(event)) {
                    SDLInputConnection.nativeCommitText(String.valueOf((char) event.getUnicodeChar()), 1);
                }
                SDLActivity.onNativeKeyDown(keyCode);
                return true;
            }
            else if (event.getAction() == KeyEvent.ACTION_UP) {
                //Log.v("SDL", "key up: " + keyCode);
                SDLActivity.onNativeKeyUp(keyCode);
                return true;
            }
        }

        if ((event.getSource() & InputDevice.SOURCE_MOUSE) != 0) {
            // on some devices key events are sent for mouse BUTTON_BACK/FORWARD presses
            // they are ignored here because sending them as mouse input to SDL is messy
            if ((keyCode == KeyEvent.KEYCODE_BACK) || (keyCode == KeyEvent.KEYCODE_FORWARD)) {
                switch (event.getAction()) {
                case KeyEvent.ACTION_DOWN:
                case KeyEvent.ACTION_UP:
                    // mark the event as handled or it will be handled by system
                    // handling KEYCODE_BACK by system will call onBackPressed()
                    return true;
                }
            }
        }

        return false;
    }

    // Touch events
    @Override
    public boolean onTouch(View v, MotionEvent event) {
        /* Ref: http://developer.android.com/training/gestures/multi.html */
        final int touchDevId = event.getDeviceId();
        final int pointerCount = event.getPointerCount();
        int action = event.getActionMasked();
        int pointerFingerId;
        int mouseButton;
        int i = -1;
        float x,y,p;

        // !!! FIXME: dump this SDK check after 2.0.4 ships and require API14.
        // 12290 = Samsung DeX mode desktop mouse
        if ((event.getSource() == InputDevice.SOURCE_MOUSE || event.getSource() == 12290) && SDLActivity.mSeparateMouseAndTouch) {
            if (Build.VERSION.SDK_INT < 14) {
                mouseButton = 1; // all mouse buttons are the left button
            } else {
                try {
                    mouseButton = (Integer) event.getClass().getMethod("getButtonState").invoke(event);
                } catch(Exception e) {
                    mouseButton = 1;    // oh well.
                }
            }

            // We need to check if we're in relative mouse mode and get the axis offset rather than the x/y values
            // if we are.  We'll leverage our existing mouse motion listener
            SDLGenericMotionListener_API12 motionListener = SDLActivity.getMotionListener();
            x = motionListener.getEventX(event);
            y = motionListener.getEventY(event);

            SDLActivity.onNativeMouse(mouseButton, action, x, y, motionListener.inRelativeMode());
        } else {
            switch(action) {
                case MotionEvent.ACTION_MOVE:
                    for (i = 0; i < pointerCount; i++) {
                        pointerFingerId = event.getPointerId(i);
                        x = event.getX(i) / mWidth;
                        y = event.getY(i) / mHeight;
                        p = event.getPressure(i);
                        if (p > 1.0f) {
                            // may be larger than 1.0f on some devices
                            // see the documentation of getPressure(i)
                            p = 1.0f;
                        }
                        SDLActivity.onNativeTouch(touchDevId, pointerFingerId, action, x, y, p);
                    }
                    break;

                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_DOWN:
                    // Primary pointer up/down, the index is always zero
                    i = 0;
                case MotionEvent.ACTION_POINTER_UP:
                case MotionEvent.ACTION_POINTER_DOWN:
                    // Non primary pointer up/down
                    if (i == -1) {
                        i = event.getActionIndex();
                    }

                    pointerFingerId = event.getPointerId(i);
                    x = event.getX(i) / mWidth;
                    y = event.getY(i) / mHeight;
                    p = event.getPressure(i);
                    if (p > 1.0f) {
                        // may be larger than 1.0f on some devices
                        // see the documentation of getPressure(i)
                        p = 1.0f;
                    }
                    SDLActivity.onNativeTouch(touchDevId, pointerFingerId, action, x, y, p);
                    break;

                case MotionEvent.ACTION_CANCEL:
                    for (i = 0; i < pointerCount; i++) {
                        pointerFingerId = event.getPointerId(i);
                        x = event.getX(i) / mWidth;
                        y = event.getY(i) / mHeight;
                        p = event.getPressure(i);
                        if (p > 1.0f) {
                            // may be larger than 1.0f on some devices
                            // see the documentation of getPressure(i)
                            p = 1.0f;
                        }
                        SDLActivity.onNativeTouch(touchDevId, pointerFingerId, MotionEvent.ACTION_UP, x, y, p);
                    }
                    break;

                default:
                    break;
            }
        }

        return true;
   }

    // Sensor events
    public void enableSensor(int sensortype, boolean enabled) {
        // TODO: This uses getDefaultSensor - what if we have >1 accels?
        if (enabled) {
            mSensorManager.registerListener(this,
                            mSensorManager.getDefaultSensor(sensortype),
                            SensorManager.SENSOR_DELAY_GAME, null);
        } else {
            mSensorManager.unregisterListener(this,
                            mSensorManager.getDefaultSensor(sensortype));
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // TODO
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {

            // Since we may have an orientation set, we won't receive onConfigurationChanged events.
            // We thus should check here.
            int newOrientation = SDLActivity.SDL_ORIENTATION_UNKNOWN;
    
            float x, y;
            switch (mDisplay.getRotation()) {
                case Surface.ROTATION_90:
                    x = -event.values[1];
                    y = event.values[0];
                    newOrientation = SDLActivity.SDL_ORIENTATION_LANDSCAPE;
                    break;
                case Surface.ROTATION_270:
                    x = event.values[1];
                    y = -event.values[0];
                    newOrientation = SDLActivity.SDL_ORIENTATION_LANDSCAPE_FLIPPED;
                    break;
                case Surface.ROTATION_180:
                    x = -event.values[1];
                    y = -event.values[0];
                    newOrientation = SDLActivity.SDL_ORIENTATION_PORTRAIT_FLIPPED;
                    break;
                default:
                    x = event.values[0];
                    y = event.values[1];
                    newOrientation = SDLActivity.SDL_ORIENTATION_PORTRAIT;
                    break;
            }
            
            float z = event.values[2];
            float g = SensorManager.GRAVITY_EARTH;
            
            if (newOrientation != SDLActivity.mCurrentOrientation) {
                SDLActivity.mCurrentOrientation = newOrientation;
                SDLActivity.onNativeOrientationChanged(newOrientation);
            }

            SDLActivity.onNativeAccel(-x / g,
                                      y / g,
                                      z / g);

            
        }
    }

    // Captured pointer events for API 26.
    public boolean onCapturedPointerEvent(MotionEvent event)
    {
        int action = event.getActionMasked();

        float x, y;
        switch (action) {
            case MotionEvent.ACTION_SCROLL:
                x = event.getAxisValue(MotionEvent.AXIS_HSCROLL, 0);
                y = event.getAxisValue(MotionEvent.AXIS_VSCROLL, 0);
                SDLActivity.onNativeMouse(0, action, x, y, false);
                return true;

            case MotionEvent.ACTION_HOVER_MOVE:
            case MotionEvent.ACTION_MOVE:
                x = event.getX(0);
                y = event.getY(0);
                SDLActivity.onNativeMouse(0, action, x, y, true);
                return true;

            case MotionEvent.ACTION_BUTTON_PRESS:
            case MotionEvent.ACTION_BUTTON_RELEASE:

                // Change our action value to what SDL's code expects.
                if (action == MotionEvent.ACTION_BUTTON_PRESS) {
                    action = MotionEvent.ACTION_DOWN;
                }
                else if (action == MotionEvent.ACTION_BUTTON_RELEASE) {
                    action = MotionEvent.ACTION_UP;
                }

                x = event.getX(0);
                y = event.getY(0);
                int button = event.getButtonState();

                SDLActivity.onNativeMouse(button, action, x, y, true);
                return true;
        }

        return false;
    }

}

