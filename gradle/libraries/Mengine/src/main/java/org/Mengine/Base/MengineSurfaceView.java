package org.Mengine.Base;

import android.content.Context;
import android.graphics.Canvas;
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

public class MengineSurfaceView extends SurfaceView implements SurfaceHolder.Callback, View.OnKeyListener, View.OnTouchListener {
    public static final String TAG = "MengineSurfaceView";

    private static native void AndroidPlatform_surfaceCreated(Surface surface);
    private static native void AndroidPlatform_surfaceDestroyed(Surface surface);
    private static native void AndroidPlatform_surfaceChanged(Surface surface, int surfaceWidth, int surfaceHeight, int deviceWidth, int deviceHeight, float rate);

    protected CountDownLatch m_runLatch;

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

        m_surfaceWidth = 0;
        m_surfaceHeight = 0;
    }

    public MengineSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
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
        /*
        Log.v("SDL", "Window size: " + width + "x" + height);
        Log.v("SDL", "Device size: " + nDeviceWidth + "x" + nDeviceHeight);
        SDLActivity.nativeSetScreenResolution(width, height, nDeviceWidth, nDeviceHeight, mDisplay.getRefreshRate());
        SDLActivity.onNativeResize();

        // Prevent a screen distortion glitch,
        // for instance when the device is in Landscape and a Portrait App is resumed.
        boolean skip = false;
        int requestedOrientation = SDLActivity.mSingleton.getRequestedOrientation();

        if (requestedOrientation == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT || requestedOrientation == ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT) {
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

        // Don't skip in MultiWindow.
        if (skip) {
            if (Build.VERSION.SDK_INT >= 24) {
                if (SDLActivity.mSingleton.isInMultiWindowMode()) {
                    Log.v("SDL", "Don't skip in Multi-Window");
                    skip = false;
                }
            }
        }

        if (skip) {
            Log.v("SDL", "Skip .. Surface is not ready.");
            mIsSurfaceReady = false;
            return;
        }

        SDLActivity.onNativeSurfaceChanged();

        mIsSurfaceReady = true;

        SDLActivity.mNextNativeState = SDLActivity.NativeState.RESUMED;
        SDLActivity.handleNativeState();
        */
    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
        MengineLog.logMessage(TAG, "onKey");

        return false;
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        MengineLog.logMessage(TAG, "onTouch");

        return false;
    }
}