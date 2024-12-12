package org.Mengine.Base;

import android.view.Surface;

import java.io.Writer;

public class MengineNative {
    public static final String TAG = "MengineNative";

    public static native boolean AndroidMain_main(Object application);
    public static native Object AndroidMain_bootstrap(String nativeLibraryDir, String[] _args);
    public static native void AndroidMain_destroy(Object application);

    public static native void AndroidMain_crash(String reason);

    public static native void AndroidEnv_setMengineAndroidActivityJNI(Object activity);
    public static native void AndroidEnv_removeMengineAndroidActivityJNI();

    public static native void AndroidEnvironmentService_log(int level, String tag, String msg);
    public static native void AnroidEnvironmentService_callMengineSemaphoreListener(Object impl);
    public static native void AnroidEnvironmentService_destroyMengineSemaphoreListener(Object impl);
    public static native boolean AndroidEnvironmentService_isDevelopmentMode();
    public static native String AndroidEnvironmentService_getCompanyName();
    public static native String AndroidEnvironmentService_getProjectName();
    public static native String AndroidEnvironmentService_getExtraPreferencesFolderName();
    public static native boolean AndroidEnvironmentService_hasCurrentAccount();
    public static native void AndroidEnvironmentService_deleteCurrentAccount();
    public static native String AndroidEnvironmentService_getCurrentAccountFolderName();
    public static native boolean AndroidEnvironmentService_writeCurrentLogToFile(Writer writer);
    public static native boolean AndroidEnvironmentService_writeOldLogToFile(Writer writer);
    public static native int AndroidEnvironmentService_getProjectVersion();

    public static native void AndroidNativePython_addPlugin(String name, Object plugin);
    public static native void AndroidNativePython_removePlugin(String name);
    public static native void AndroidNativePython_call(String plugin, String method, Object []args);

    public static native void AndroidPlatform_surfaceCreatedEvent(Surface surface);
    public static native void AndroidPlatform_surfaceDestroyedEvent(Surface surface);
    public static native void AndroidPlatform_surfaceChangedEvent(Surface surface, int surfaceWidth, int surfaceHeight, int deviceWidth, int deviceHeight, float rate);
    public static native void AndroidPlatform_keyEvent(boolean isDown, int keyCode, int repeatCount);
    public static native void AndroidPlatform_textEvent(int unicode);
    public static native void AndroidPlatform_touchEvent(int action, int pointerId, float x, float y, float pressure);
    public static native void AndroidPlatform_accelerationEvent(float x, float y, float z);
    public static native void AndroidPlatform_pauseEvent();
    public static native void AndroidPlatform_resumeEvent();
    public static native void AndroidPlatform_stopEvent();
    public static native void AndroidPlatform_startEvent();
    public static native void AndroidPlatform_clipboardChangedEvent();
    public static native void AndroidPlatform_windowFocusChangedEvent(boolean focus);
    public static native void AndroidPlatform_quitEvent();
    public static native void AndroidPlatform_lowMemory();
    public static native void AndroidPlatform_trimMemory(int level);
    public static native void AndroidPlatform_changeLocale(String locale);

    public static native void AndroidEnv_setMengineAndroidApplicationJNI(Object activity, ClassLoader cl);
    public static native void AndroidEnv_removeMengineAndroidApplicationJNI();

    public static native boolean AndroidEnv_isMasterRelease();
    public static native String AndroidEnv_getEngineGITSHA1();
    public static native String AndroidEnv_getEngineVersion();
    public static native String AndroidEnv_getBuildDate();
    public static native String AndroidEnv_getBuildUsername();

    public static native long AndroidStatistic_getRenderFrameCount();
}