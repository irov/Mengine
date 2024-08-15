package org.Mengine.Base;

import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Looper;
import android.os.Parcelable;
import android.view.KeyEvent;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.RelativeLayout;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.List;
import java.util.concurrent.CountDownLatch;

public class MengineActivity extends AppCompatActivity {
    public static final String TAG = "MengineActivity";

    private static native Object AndroidMain_bootstrap(String nativeLibraryDir, String[] _args);
    private static native void AndroidMain_destroy(Object application);

    private static native void AndroidEnv_setMengineAndroidActivityJNI(Object activity);
    private static native void AndroidEnv_removeMengineAndroidActivityJNI();

    private static native void AndroidEnvironmentService_quitMengineAndroidActivityJNI();
    private static native boolean AndroidEnvironmentService_isDevelopmentMode();
    private static native String AndroidEnvironmentService_getCompanyName();
    private static native String AndroidEnvironmentService_getProjectName();
    private static native String AndroidEnvironmentService_getExtraPreferencesFolderName();
    private static native boolean AndroidEnvironmentService_hasCurrentAccount();
    private static native String AndroidEnvironmentService_getCurrentAccountFolderName();
    private static native boolean AndroidEnvironmentService_writeCurrentLogToFile(Writer writer);
    private static native boolean AndroidEnvironmentService_writeOldLogToFile(Writer writer);
    private static native int AndroidEnvironmentService_getProjectVersion();

    private static native void AndroidNativePython_addPlugin(String name, Object plugin);
    private static native void AndroidNativePython_call(String plugin, String method, Object []args);

    public static native void AndroidPlatform_surfaceCreatedEvent(Surface surface);
    public static native void AndroidPlatform_surfaceDestroyedEvent(Surface surface);
    public static native void AndroidPlatform_surfaceChangedEvent(Surface surface, int surfaceWidth, int surfaceHeight, int deviceWidth, int deviceHeight, float rate);
    public static native void AndroidPlatform_keyEvent(boolean isDown, int keyCode, int repeatCount);
    public static native void AndroidPlatform_textEvent(int unicode);
    public static native void AndroidPlatform_touchEvent(int action, int pointerId, float x, float y, float pressure);
    public static native void AndroidPlatform_accelerationEvent(float x, float y, float z);
    public static native void AndroidPlatform_pauseEvent();
    public static native void AndroidPlatform_resumeEvent();
    public static native void AndroidPlatform_clipboardChangedEvent();
    public static native void AndroidPlatform_windowFocusChangedEvent( boolean focus );
    public static native void AndroidPlatform_quitEvent();

    private boolean m_initializePython;
    private boolean m_destroy;

    private Object m_nativeApplication;

    private static Map<String, Integer> m_requestCodes = new HashMap<>();
    private Map<String, MengineSemaphore> m_semaphores;

    private Thread m_threadMain;

    private MengineCommandHandler m_commandHandler;

    private RelativeLayout m_contentView;

    private MengineSurfaceView m_surfaceView;

    private MengineSoftInput m_softInput;

    private MengineClipboard m_clipboard;

    public MengineActivity() {
        super();
    }

    public boolean isDevelopmentMode() {
        return AndroidEnvironmentService_isDevelopmentMode();
    }

    public String getCompanyName() {
       return AndroidEnvironmentService_getCompanyName();
    }

    public String getProjectName() {
        return AndroidEnvironmentService_getProjectName();
    }

    public int getProjectVersion() {
        return AndroidEnvironmentService_getProjectVersion();
    }

    public MengineApplication getMengineApplication() {
        MengineApplication application = (MengineApplication)this.getApplication();

        return application;
    }

    public String getApplicationId() {
        MengineApplication application = this.getMengineApplication();

        String applicationId = application.getApplicationId();

        return applicationId;
    }

    protected List<MenginePlugin> getPlugins() {
        MengineApplication application = this.getMengineApplication();

        List<MenginePlugin> plugins = application.getPlugins();

        return plugins;
    }

    protected List<MenginePluginKeyListener> getKeyListeners() {
        MengineApplication application = this.getMengineApplication();

        List<MenginePluginKeyListener> listeners = application.getKeyListeners();

        return listeners;
    }

    protected List<MenginePluginActivityListener> getActivityListeners() {
        MengineApplication application = this.getMengineApplication();

        List<MenginePluginActivityListener> listeners = application.getActivityListeners();

        return listeners;
    }

    protected List<MenginePluginEngineListener> getEngineListeners() {
        MengineApplication application = this.getMengineApplication();

        List<MenginePluginEngineListener> listeners = application.getEngineListeners();

        return listeners;
    }

    @SuppressWarnings("unchecked")
    public <T> T getPlugin(Class<T> cls) {
        if (m_destroy == true) {
            return null;
        }

        MengineApplication application = this.getMengineApplication();

        T plugin = application.getPlugin(cls);

        return plugin;
    }

    public String getSessionId() {
        if (m_destroy == true) {
            return "";
        }

        MengineApplication application = this.getMengineApplication();

        String sessionId = application.getSessionId();

        return sessionId;
    }

    public String getVersionName() {
        if (m_destroy == true) {
            return "";
        }

        MengineApplication application = this.getMengineApplication();

        String versionName = application.getVersionName();

        return versionName;
    }

    public void setState(String name, Object value) {
        if (m_destroy == true) {
            return;
        }

        MengineApplication application = this.getMengineApplication();

        application.setState(name, value);
    }

    public boolean hasMetaData(String name) {
        if (m_destroy == true) {
            return false;
        }

        MengineApplication application = this.getMengineApplication();

        boolean result = application.hasMetaData(name);

        return result;
    }

    public String getMetaDataString(String name) {
        if (m_destroy == true) {
            return null;
        }

        MengineApplication application = this.getMengineApplication();

        String value = application.getMetaDataString(name);

        return value;
    }

    public boolean getMetaDataBoolean(String name) {
        if (m_destroy == true) {
            return false;
        }

        MengineApplication application = this.getMengineApplication();

        boolean value = application.getMetaDataBoolean(name);

        return value;
    }

    public int getMetaDataInteger(String name) {
        if (m_destroy == true) {
            return 0;
        }

        MengineApplication application = this.getMengineApplication();

        int value = application.getMetaDataInteger(name);

        return value;
    }

    public ViewGroup getContentViewGroup() {
        ViewGroup viewGroup = (ViewGroup)m_contentView;

        return viewGroup;
    }

    public Surface getSurface() {
        if (m_surfaceView == null) {
            return null;
        }

        Surface surface = m_surfaceView.getSurface();

        return surface;
    }

    MengineSurfaceView getSurfaceView() {
        return m_surfaceView;
    }

    protected void finishWithAlertDialog(String format, Object... args) {
        MengineUtils.finishActivityWithAlertDialog(this, format, args);
    }

    public MengineTransparencyConsentParam makeTransparencyConsentParam() {
        MengineApplication application = this.getMengineApplication();

        MengineTransparencyConsentParam consent = application.makeTransparencyConsentParam();

        return consent;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        try {
            Thread.currentThread().setName("MengineActivity");
        } catch (Exception e) {
            MengineLog.logMessage(TAG, "modify activity thread name exception: %s"
                , e.getMessage()
            );
        }

        ApplicationInfo applicationInfo = application.getApplicationInfo();
        String nativeLibraryDir = applicationInfo.nativeLibraryDir;
        String options = application.getApplicationOptions();

        if (application.isInvalidInitialize() == true) {
            this.finishWithAlertDialog("[ERROR] Application invalid initialize");

            return;
        }

        MengineLog.logMessageRelease(TAG, "onCreate");

        this.setState("activity.lifecycle", "create");
        this.setState("activity.init", "begin");

        m_initializePython = false;
        m_destroy = false;

        m_commandHandler = new MengineCommandHandler(Looper.getMainLooper(), this);
        m_semaphores = new HashMap<>();

        this.setState("activity.init", "setup_orientation");

        int orientation = getResources().getInteger(R.integer.app_screen_orientation);

        this.setRequestedOrientation(orientation);

        this.setState("activity.init", "setup_relativelayout");

        RelativeLayout contentView = new RelativeLayout(this);

        m_contentView = contentView;

        this.setState("activity.init", "setup_surface");

        CountDownLatch runLatch = new CountDownLatch(1);

        MengineSurfaceView surface = new MengineSurfaceView(this, runLatch);

        m_surfaceView = surface;

        m_contentView.addView(m_surfaceView);

        this.setState("activity.init", "setup_softinput");

        MengineSoftInput softInput = new MengineSoftInput(this, m_surfaceView);

        m_softInput = softInput;

        m_contentView.addView(m_softInput);

        this.setState("activity.init", "setup_contentview");

        this.setContentView(m_contentView);

        this.setState("activity.init", "setup_clipboard");

        MengineClipboard clipboard = new MengineClipboard(this);

        m_clipboard = clipboard;

        this.setState("activity.init", "bootstrap");

        AndroidEnv_setMengineAndroidActivityJNI(this);

        ApplicationInfo applicationInfo = application.getApplicationInfo();
        String nativeLibraryDir = applicationInfo.nativeLibraryDir;
        String options = application.getApplicationOptions();

        String[] optinsArgs = options.split(" ");

        m_nativeApplication = AndroidMain_bootstrap(nativeLibraryDir, optinsArgs);

        this.setState("activity.init", "plugin_create");

        List<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.setActivity(this);
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logMessage(TAG, "onCreate plugin: %s"
                , l.getPluginName()
            );

            try {
                l.onCreate(this, savedInstanceState);
            } catch (MenginePluginInvalidInitializeException e) {
                this.setState("activity.init", "plugin_create_exception." + l.getPluginName());

                MengineAnalytics.buildEvent("mng_activity_init_failed")
                    .addParameterException("reason", e)
                    .logAndFlush();

                this.finishWithAlertDialog("[ERROR] onCreate plugin: %s exception: %s"
                    , l.getPluginName()
                    , e.getMessage()
                );

                return;
            }
        }

        this.setState("activity.init", "setup_main");

        MengineMain main = new MengineMain(this, m_nativeApplication, runLatch);

        m_threadMain = new Thread(main);
        m_threadMain.start();

        this.setState("activity.init", "end");

        this.setState("activity.lifecycle", "created");
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);

        this.setState("activity.lifecycle", "post_create");

        MengineLog.logMessage(TAG, "onPostCreate");
    }

    @SuppressWarnings("deprecation")
    private void syncFullscreenWindow() {
        Window window = this.getWindow();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            WindowInsetsController insetsController = window.getInsetsController();
            if (insetsController == null) {
                MengineLog.logError(TAG, "insets controller is null");

                return;
            }

            insetsController.hide(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
            insetsController.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
        } else {
            int flags = View.SYSTEM_UI_FLAG_FULLSCREEN |
                    View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                    View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
                    View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                    View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.INVISIBLE;

            View decorView = window.getDecorView();
            decorView.setSystemUiVisibility(flags);

            window.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
            window.clearFlags(WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN);
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        this.setState("activity.lifecycle", "window_focus_changed:" + (hasFocus == true ? "true" : "false"));

        MengineLog.logMessage(TAG, "onWindowFocusChanged focus: %s"
            , hasFocus
        );

        if (hasFocus == true) {
            this.syncFullscreenWindow();
        }

        MengineActivity.AndroidPlatform_windowFocusChangedEvent(hasFocus);
    }

    @Override
    public void onAttachedToWindow() {
        super.onAttachedToWindow();

        this.setState("activity.lifecycle", "attached_to_window");

        MengineLog.logMessage(TAG, "onAttachedToWindow");

        this.syncFullscreenWindow();
    }

    @Override
    public void onDetachedFromWindow() {
        super.onDetachedFromWindow();

        this.setState("activity.lifecycle", "detached_from_window");

        MengineLog.logMessage(TAG, "onDetachedFromWindow");
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        this.setState("activity.lifecycle", "save_instance_state");

        MengineLog.logMessage(TAG, "onSaveInstanceState");
    }

    @Override
    public void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);

        this.setState("activity.lifecycle", "restore_instance_state");

        MengineLog.logMessage(TAG, "onRestoreInstanceState: %s"
            , savedInstanceState
        );
    }

    public void quitMengineApplication() {
        AndroidEnvironmentService_quitMengineAndroidActivityJNI();
    }

    public void onMengineInitializeBaseServices() {
        MengineLog.logInfo(TAG, "onMengineInitializeBaseServices");

        MengineLog.initialize(this);

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onMengineInitializeBaseServices(this);
        }
    }

    public void onMengineCreateApplication() {
        MengineLog.logInfo(TAG, "onMengineCreateApplication");

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onMengineCreateApplication(this);
        }
    }

    public void onMenginePlatformRun() {
        MengineLog.logInfo(TAG, "onMenginePlatformRun");

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onMenginePlatformRun(this);
        }
    }

    public void onMenginePlatformStop() {
        MengineLog.logInfo(TAG, "onMenginePlatformStop");

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onMenginePlatformStop(this);
        }

        MengineLog.finalize(this);
    }

    public void onMengineCurrentSceneChange(String name) {
        MengineLog.logInfo(TAG, "onMengineCurrentSceneChange: %s"
            , name
        );

        this.setState("current.scene", name);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
    	this.setState("activity.result", "request: " + requestCode + "result: " + resultCode );
    	
        MengineLog.logMessageRelease(TAG, "onActivityResult request: %d result: %d"
            , requestCode
            , resultCode
        );

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onActivityResultBefore(this, requestCode, resultCode, data);
        }

        super.onActivityResult(requestCode, resultCode, data);

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onActivityResult(this, requestCode, resultCode, data);
        }
    }

    @Override
    public void onStart() {
        super.onStart();

        this.setState("activity.lifecycle", "start");

        MengineLog.logMessageRelease(TAG, "onStart");

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onStart(this);
        }

        this.setState("activity.lifecycle", "started");
    }

    @Override
    public void onStop() {
        super.onStop();

        this.setState("activity.lifecycle", "stop");

        MengineLog.logMessageRelease(TAG, "onStop");

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onStop(this);
        }

        this.setState("activity.lifecycle", "stoped");
    }

    @Override
    public void onPause() {
        super.onPause();

        this.setState("activity.lifecycle", "pause");

        MengineLog.logMessage(TAG, "onPause");

        if (m_surfaceView != null) {
            m_surfaceView.handlePause();
        }

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onPause(this);
        }

        this.setState("activity.lifecycle", "paused");
    }

    @Override
    public void onResume() {
        super.onResume();

        this.setState("activity.lifecycle", "resume");

        MengineLog.logMessage(TAG, "onResume");

        if (m_surfaceView != null) {
            m_surfaceView.handleResume();
        }

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onResume(this);
        }

        this.setState("activity.lifecycle", "resumed");
    }

    @Override
    public void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        this.setState("activity.intent_action", intent.getAction() );

        MengineLog.logMessageRelease(TAG, "onNewIntent intent: %s", intent);

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onNewIntent(this, intent);
        }
    }

    @Override
    public void onDestroy() {
        if (m_destroy == true) {
            MengineLog.logWarning(TAG, "onDestroy: already destroyed");

            super.onDestroy();

            return;
        }

        m_destroy = true;

        this.setState("activity.lifecycle", "destroy");

        MengineLog.logMessageRelease(TAG, "onDestroy");

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onDestroy: application invalid initialize");

            super.onDestroy();

            return;
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onDestroy(this);
        }

        List<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.setActivity(null);
        }

        AndroidPlatform_quitEvent();

        try {
            m_threadMain.join();
        } catch (InterruptedException e) {
            MengineLog.logError(TAG, "thread main join exception: %s"
                , e.getMessage()
            );
        }

        AndroidMain_destroy(m_nativeApplication);
        m_nativeApplication = null;

        try {
            m_threadMain.join();
        } catch (InterruptedException e) {
            MengineLog.logError(TAG, "thread main join exception: %s"
                , e.getMessage()
            );
        }

        AndroidEnv_removeMengineAndroidActivityJNI();

        m_semaphores = null;
        m_commandHandler = null;

        super.onDestroy();
    }

    @Override
    public void onRestart() {
        super.onRestart();

        this.setState("activity.lifecycle", "restart");

        MengineLog.logMessage(TAG, "onRestart");

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onRestart(this);
        }

        this.setState("activity.lifecycle", "restarted");
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();

        this.setState("activity.low_memory", true);

        MengineLog.logMessage(TAG, "onLowMemory");
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        this.setState("configuration.orientation", newConfig.orientation);

        MengineLog.logMessage(TAG, "onConfigurationChanged config: %s"
            , newConfig.toString()
        );

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onConfigurationChanged(this, newConfig);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        MengineLog.logMessage(TAG, "onRequestPermissionsResult request: %d permissions: %s grantResults: %s"
            , requestCode
            , Arrays.toString(permissions)
            , Arrays.toString(grantResults)
        );

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onRequestPermissionsResult(this, requestCode, permissions, grantResults);
        }
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        MengineLog.logInfo(TAG, "dispatchKeyEvent action: %d key: %d scan: %d"
            , event.getAction()
            , event.getKeyCode()
            , event.getScanCode()
        );

        MengineApplication application = (MengineApplication)this.getApplication();

        List<MenginePluginKeyListener> listeners = this.getKeyListeners();

        for (MenginePluginKeyListener l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            if (l.dispatchKeyEvent(this, event) == true) {
                return true;
            }
        }

        return super.dispatchKeyEvent(event);
    }

    /***********************************************************************************************
     * Kernel Methods
     **********************************************************************************************/

    public static int genRequestCode(String name) {
        if (m_requestCodes.containsKey(name) == false) {
            int new_code = m_requestCodes.size();

            m_requestCodes.put(name, new_code);

            return new_code;
        }

        int code = m_requestCodes.get(name);

        return code;
    }

    /***********************************************************************************************
     * Python Methods
     **********************************************************************************************/

    public void onPythonEmbeddedInitialize() {
        MengineLog.logInfo(TAG, "onPythonEmbeddedInitialize");

        m_initializePython = true;

        AndroidNativePython_addPlugin("Activity", this);

        MengineApplication application = (MengineApplication)this.getApplication();
        AndroidNativePython_addPlugin("Application", application);

        List<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            if (p.onAvailable(application) == false) {
                continue;
            }

            try {
                Class<?> cls = p.getClass();

                Field PLUGIN_EMBEDDING = cls.getField("PLUGIN_EMBEDDING");

                if( PLUGIN_EMBEDDING.getBoolean(null) == false ) {
                    continue;
                }
            } catch (NoSuchFieldException ex) {
                continue;
            } catch (IllegalAccessException ex) {
                continue;
            }

            String name = p.getPluginName();

            this.addPythonPlugin(name, p);
        }
    }

    public void onPythonEmbeddedFinalize() {
        m_initializePython = false;
    }

    public void pythonCall(String plugin, String method, Object ... args) {
        if (m_initializePython == false) {
            MengineLog.logWarning(TAG,"pythonCall call before embedding plugin: %s method: %s args: %s"
                , plugin
                , method
                , Arrays.toString(args)
            );

            return;
        }

        if (BuildConfig.DEBUG == true) {
            MengineLog.logInfo(TAG, "pythonCall plugin [%s] method [%s] args [%s]"
                , plugin
                , method
                , Arrays.toString(args)
            );
        }

        this.setState("python.call", plugin + "." + method);

        AndroidNativePython_call(plugin, method, args);
    }

    public void addPythonPlugin(String name, Object plugin) {
        if (m_destroy == true) {
            return;
        }

        MengineLog.logMessage(TAG, "addPythonPlugin name: %s plugin: %s"
            , name
            , plugin
        );

        AndroidNativePython_addPlugin(name, plugin);
    }

    /***********************************************************************************************
     * Semaphore Methods
     **********************************************************************************************/

    public void activateSemaphore(String name) {
        if (m_destroy == true) {
            return;
        }

        MengineLog.logMessage(TAG, "activateSemaphore semaphore: %s"
            , name
        );

        this.setState("python.semaphore", name);

        ArrayList<MengineSemaphoreListener> activate_listeners = null;

        synchronized (m_semaphores) {
            MengineSemaphore semaphore = m_semaphores.get(name);

            if (semaphore == null) {
                semaphore = new MengineSemaphore(true);

                m_semaphores.put(name, semaphore);

                return;
            }

            if (semaphore.isActivated() == true) {
                return;
            }

            activate_listeners = semaphore.activate();
        }

        final ArrayList<MengineSemaphoreListener> final_activate_listeners = activate_listeners;

        MengineUtils.performOnMainThread(() -> {
            for (MengineSemaphoreListener listener : final_activate_listeners) {
                listener.call();
            }
        });
    }

    public void deactivateSemaphore(String name) {
        if (m_destroy == true) {
            return;
        }

        MengineLog.logMessage(TAG, "deactivateSemaphore semaphore: %s"
            , name
        );

        synchronized (m_semaphores) {
            m_semaphores.remove(name);
        }
    }

    public void waitSemaphore(String name, MengineSemaphoreListener cb) {
        if (m_destroy == true) {
            return;
        }

        MengineLog.logMessage(TAG, "waitSemaphore semaphore: %s"
            , name
        );

        synchronized (m_semaphores) {
            MengineSemaphore semaphore = m_semaphores.get(name);

            if (semaphore == null) {
                semaphore = new MengineSemaphore(false);
                semaphore.addListener(cb);

                m_semaphores.put(name, semaphore);
                return;
            }

            if (semaphore.isActivated() == false) {
                semaphore.addListener(cb);
                return;
            }
        }

        MengineUtils.performOnMainThread(() -> {
            cb.call();
        });
    }

    /***********************************************************************************************
     * Keyboard Methods
     **********************************************************************************************/

    public void showKeyboard() {
        MengineLog.logMessage(TAG, "showKeyboard");

        m_commandHandler.post(() -> {
            m_softInput.showKeyboard();
        });
    }

    public void hideKeyboard() {
        MengineLog.logMessage(TAG, "hideKeyboard");

        m_commandHandler.post(() -> {
            m_softInput.hideKeyboard();
        });
    }

    public boolean isShowKeyboard() {
        return m_softInput.isShowKeyboard();
    }

    /***********************************************************************************************
     * Clipboard Methods
     **********************************************************************************************/

    public boolean hasClipboardText() {
        boolean result = m_clipboard.hasText();

        return result;
    }

    public String getClipboardText() {
        String text = m_clipboard.getText();

        return text;
    }

    public void setClipboardText(String text) {
        m_clipboard.setText(text);
    }

    /***********************************************************************************************
     * Alert Methods
     **********************************************************************************************/

    public void showAlertDialog(String title, String format, Object ... args) {
        MengineUtils.showAlertDialogWithCb(this, () -> {}, title, format, args);
    }

    /***********************************************************************************************
     * Linking Methods
     **********************************************************************************************/

    public boolean linkingOpenURL(String url) {
        MengineLog.logMessage(TAG, "linkingOpenURL url: %s"
            , url
        );

        this.setState("open.url", url);

        MengineAnalytics.buildEvent("mng_open_url")
            .addParameterString("url", url)
            .log();

        if (MengineUtils.openUrl(this, url) == false) {
            return false;
        }

        return true;
    }

    public boolean linkingOpenMail(String email, String subject, String body) {
        MengineLog.logMessage(TAG, "linkingOpenMail mail: %s subject: %s body: %s"
            , email
            , subject
            , body
        );

        this.setState("open.mail", email);

        MengineAnalytics.buildEvent("mng_open_mail")
            .addParameterString("mail", email)
            .log();

        Context context = this.getApplicationContext();

        Intent intent = new Intent(Intent.ACTION_SEND_MULTIPLE);

        intent.setType("application/zip");

        try {
            ArrayList<Parcelable> fileUris = new ArrayList<>();

            boolean hasAccount = AndroidEnvironmentService_hasCurrentAccount();

            if (hasAccount == true) {
                String extraPreferencesFolderName = AndroidEnvironmentService_getExtraPreferencesFolderName();
                String accountFolderName = AndroidEnvironmentService_getCurrentAccountFolderName();

                File filesDir = context.getFilesDir();

                File extraPreferencesFolder = new File(filesDir, extraPreferencesFolderName);
                File accountFolder = new File(extraPreferencesFolder, accountFolderName);

                File accountZipFile = MengineUtils.createTempFile(context, "mng_account_", ".zip");

                if (accountZipFile == null) {
                    MengineLog.logWarning(TAG, "linkingOpenMail invalid create temp file 'mng_account_***.zip' for mail: %s subject: %s"
                        , email
                        , subject
                    );

                    return false;
                }

                if (MengineUtils.zipFiles(accountFolder, accountZipFile) == true) {
                    Uri accountZIPUri = MengineUtils.getUriForFile(context, accountZipFile);

                    if (accountZIPUri == null) {
                        return false;
                    }

                    MengineLog.logInfo(TAG, "linkingOpenMail attach file '%s' for mail: %s subject: %s"
                        , accountZIPUri
                        , email
                        , subject
                    );

                    fileUris.add(accountZIPUri);
                } else {
                    body += "\n\n[ERROR] invalid zip account folder";

                    MengineLog.logWarning(TAG, "linkingOpenMail invalid zip account folder for mail: %s subject: %s"
                        , email
                        , subject
                    );
                }
            }

            File logFile = MengineUtils.createTempFile(context, "mng_log_", ".log");

            if (logFile == null) {
                MengineLog.logWarning(TAG, "linkingOpenMail invalid create temp file 'mng_log_***.log' for mail: %s subject: %s"
                    , email
                    , subject
                );

                return false;
            }

            OutputStreamWriter logFileStream = new OutputStreamWriter(new FileOutputStream(logFile));

            logFileStream.write("[BEGIN CURRENT LOG]\n\n");

            if (AndroidEnvironmentService_writeCurrentLogToFile(logFileStream) == true) {
                logFileStream.write("\n\n[END CURRENT LOG]");
                logFileStream.flush();
                logFileStream.close();

                File logZipFile = MengineUtils.createTempFile(context, "mng_log_", ".zip");

                if (logZipFile == null) {
                    MengineLog.logWarning(TAG, "linkingOpenMail invalid create temp file 'mng_log_***.zip' for mail: %s subject: %s"
                        , email
                        , subject
                    );

                    return false;
                }

                if (MengineUtils.zipFiles(logFile, logZipFile) == true) {
                    Uri logZipFileUri = MengineUtils.getUriForFile(context, logZipFile);

                    if (logZipFileUri == null) {
                        return false;
                    }

                    MengineLog.logInfo(TAG, "linkingOpenMail attach file '%s' for mail: %s subject: %s"
                        , logZipFileUri
                        , email
                        , subject
                    );

                    fileUris.add(logZipFileUri);
                } else {
                    body += "\n\n[ERROR] invalid zip current log file";

                    MengineLog.logMessage(TAG, "linkingOpenMail invalid zip current log file for mail: %s subject: %s"
                        , email
                        , subject
                    );
                }
            } else {
                body += "\n\n[ERROR] invalid write current log file";

                MengineLog.logMessage(TAG, "linkingOpenMail invalid write current log file for mail: %s subject: %s"
                    , email
                    , subject
                );
            }

            File oldLogFile = MengineUtils.createTempFile(context, "mng_old_log_", ".log");

            if (oldLogFile == null) {
                MengineLog.logWarning(TAG, "linkingOpenMail invalid create temp file 'mng_old_log_***.log' for mail: %s subject: %s"
                    , email
                    , subject
                );

                return false;
            }

            OutputStreamWriter oldLogFileStream = new OutputStreamWriter(new FileOutputStream(oldLogFile));

            oldLogFileStream.write("[BEGIN OLD LOG]\n\n");

            if (AndroidEnvironmentService_writeOldLogToFile(oldLogFileStream) == true) {
                oldLogFileStream.write("\n\n[END OLD LOG]");
                oldLogFileStream.flush();
                oldLogFileStream.close();

                File oldLogZipFile = MengineUtils.createTempFile(context, "mng_old_log_", ".zip");

                if (MengineUtils.zipFiles(oldLogFile, oldLogZipFile) == true) {
                    Uri oldLogZipFileUri = MengineUtils.getUriForFile(context, oldLogZipFile);

                    if (oldLogZipFileUri == null) {
                        return false;
                    }

                    MengineLog.logInfo(TAG, "linkingOpenMail attach file '%s' for mail: %s subject: %s"
                        , oldLogZipFileUri
                        , email
                        , subject
                    );

                    fileUris.add(oldLogZipFileUri);
                } else {
                    body += "\n\n[ERROR] invalid zip old log file";

                    MengineLog.logMessage(TAG, "linkingOpenMail invalid zip old log file for mail: %s subject: %s"
                        , email
                        , subject
                    );
                }
            } else {
                body += "\n\nNOT_FOUND_OLD_LOG";

                MengineLog.logMessage(TAG, "linkingOpenMail not found old log file for mail: %s subject: %s"
                    , email
                    , subject
                );
            }

            intent.putParcelableArrayListExtra(Intent.EXTRA_STREAM, fileUris);
        } catch (IOException e) {
            body += "\n\n[ERROR] invalid attachs file";

            MengineLog.logError(TAG, "[ERROR] linkingOpenMail failed attachs file mail: %s subject: %s exception: %s"
                , email
                , subject
                , e.getMessage()
            );
        }

        intent.putExtra(Intent.EXTRA_EMAIL, new String[] { email });
        intent.putExtra(Intent.EXTRA_SUBJECT, subject);
        intent.putExtra(Intent.EXTRA_TEXT, body);

        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        intent.addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION);

        Intent chooser = Intent.createChooser(intent, "Send Email");

        chooser.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

        try {
            this.startActivity(chooser);
        } catch (Exception e) {
            MengineLog.logError(TAG, "[ERROR] linkingOpenMail failed start mail: %s subject: %s exception: %s"
                , email
                , subject
                , e.getMessage()
            );

            return false;
        }

        return true;
    }
}
