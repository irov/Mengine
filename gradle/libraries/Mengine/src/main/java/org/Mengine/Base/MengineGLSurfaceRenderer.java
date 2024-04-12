package org.Mengine.Base;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MengineGLSurfaceRenderer implements GLSurfaceView.Renderer {
    public static final String TAG = "MengineGLSurfaceR";

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        MengineLog.logMessage( TAG,"MengineGLSurfaceRenderer::onSurfaceCreated");
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        MengineLog.logMessage(TAG, "MengineGLSurfaceRenderer::onSurfaceChanged");
    }

    public void onDrawFrame(GL10 gl) {
        MengineLog.logMessage(TAG, "MengineGLSurfaceRenderer::onDrawFrame");
    }
}