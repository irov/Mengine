package org.Mengine.Base;

import android.content.Context;
import android.graphics.Canvas;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.MotionEvent;

import androidx.annotation.NonNull;

public class MengineGLSurfaceView extends GLSurfaceView {
    public static final String TAG = "MengineGLSurfaceView";

    public MengineGLSurfaceView(Context context) {
        super(context);
    }

    public MengineGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);

        MengineLog.logMessage(TAG, "MengineGLSurfaceView::onSizeChanged");
    }

    @Override
    protected void onDraw(@NonNull Canvas canvas) {
        super.onDraw(canvas);

        MengineLog.logMessage(TAG, "MengineGLSurfaceView::onDraw");
    }

    @Override
    public boolean onTrackballEvent(MotionEvent event) {
        MengineLog.logMessage(TAG, "MengineGLSurfaceView::onTrackballEvent");

        return super.onTrackballEvent(event);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        MengineLog.logMessage(TAG, "MengineGLSurfaceView::onTouchEvent");

        return super.onTouchEvent(event);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        MengineLog.logMessage(TAG, "MengineGLSurfaceView::onKeyDown");

        return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        MengineLog.logMessage(TAG, "MengineGLSurfaceView::onKeyUp");

        return super.onKeyUp(keyCode, event);
    }
}