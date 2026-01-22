package org.Mengine.Base;

import android.view.Surface;

import java.io.File;
import java.io.Writer;

public class MengineNative {
    public static final MengineTag TAG = MengineTag.of("MNGNative");

    public static native boolean AndroidMain_main(Object application);
    public static native Object AndroidMain_bootstrap(String nativeLibraryDir, String[] _args);
    public static native void AndroidMain_destroy(Object application);

    public static native void AndroidMain_crash(String reason);

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

    public static native void AndroidKernelService_addPlugin(String plugin, Object module);
    public static native void AndroidKernelService_removePlugin(String plugin);
    public static native void AndroidKernelService_call(String plugin, String method, Object []args);
    public static native void AndroidKernelService_activateSemaphore(String semaphore);

    public static native void AndroidPlatform_surfaceCreatedEvent(Surface surface);
    public static native void AndroidPlatform_surfaceDestroyedEvent();
    public static native void AndroidPlatform_surfaceChangedEvent(Surface surface, int surfaceWidth, int surfaceHeight, int deviceWidth, int deviceHeight, float rate);
    public static native void AndroidPlatform_keyEvent(boolean isDown, int keyCode, int repeatCount);
    public static native void AndroidPlatform_textEvent(int unicode);
    public static native void AndroidPlatform_touchEvent(int action, int pointerId, float x, float y, float pressure);
    public static native void AndroidPlatform_accelerationEvent(float x, float y, float z);
    public static native void AndroidPlatform_pauseEvent(float x, float y);
    public static native void AndroidPlatform_resumeEvent(float x, float y);
    public static native float AndroidPlatform_getLastFingerX();
    public static native float AndroidPlatform_getLastFingerY();
    public static native float AndroidPlatform_getLastFingerPressure();
    public static native void AndroidPlatform_stopEvent();
    public static native void AndroidPlatform_startEvent();
    public static native void AndroidPlatform_restartEvent();
    public static native void AndroidPlatform_destroyEvent();
    public static native void AndroidPlatform_freezeEvent(String owner, boolean freeze);
    public static native void AndroidPlatform_clipboardChangedEvent();
    public static native void AndroidPlatform_windowFocusChangedEvent(boolean focus);
    public static native void AndroidPlatform_quitEvent();
    public static native void AndroidPlatform_lowMemory();
    public static native void AndroidPlatform_trimMemory(int level);
    public static native void AndroidPlatform_changeLocale(String locale);

    public static native void AndroidPlatform_lockActivity();
    public static native void AndroidPlatform_unlockActivity();

    public static native void AndroidEnv_setMengineAndroidClassLoaderJNI(ClassLoader cl);
    public static native void AndroidEnv_removeMengineAndroidClassLoaderJNI();
    public static native boolean AndroidEnv_isMasterRelease();
    public static native String AndroidEnv_getEngineGITSHA1();
    public static native String AndroidEnv_getEngineVersion();
    public static native String AndroidEnv_getBuildDate();
    public static native String AndroidEnv_getBuildUsername();

    public static native long AndroidStatistic_getRenderFrameCount();
    public static native long AndroidStatistic_getAllocatorSize();
    public static native long AndroidStatistic_getRenderTextureAllocSize();
}