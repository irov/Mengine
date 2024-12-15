package org.Mengine.Base;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
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
import android.widget.RelativeLayout;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.List;

public class MengineActivity extends AppCompatActivity {
    public static final String TAG = "MengineActivity";

    private Locale m_currentLocale;

    private static final Map<String, Integer> m_requestCodes = new HashMap<>();

    private static final Object m_syncronizationSemaphores = new Object();
    private Map<String, MengineSemaphore> m_semaphores;

    private MengineCommandHandler m_commandHandler;

    private RelativeLayout m_contentView;

    private MengineSurfaceView m_surfaceView;

    private MengineSoftInput m_softInput;

    private MengineClipboard m_clipboard;

    public MengineActivity() {
        super();
    }

    public boolean isDevelopmentMode() {
        return MengineNative.AndroidEnvironmentService_isDevelopmentMode();
    }

    public String getCompanyName() {
       return MengineNative.AndroidEnvironmentService_getCompanyName();
    }

    public String getProjectName() {
        return MengineNative.AndroidEnvironmentService_getProjectName();
    }

    public int getProjectVersion() {
        return MengineNative.AndroidEnvironmentService_getProjectVersion();
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

    protected List<MengineService> getPlugins() {
        MengineApplication application = this.getMengineApplication();

        List<MengineService> plugins = application.getServices();

        return plugins;
    }

    protected List<MengineListenerKeyEvent> getKeyListeners() {
        MengineApplication application = this.getMengineApplication();

        List<MengineListenerKeyEvent> listeners = application.getKeyListeners();

        return listeners;
    }

    protected List<MengineListenerActivity> getActivityListeners() {
        MengineApplication application = this.getMengineApplication();

        List<MengineListenerActivity> listeners = application.getActivityListeners();

        return listeners;
    }

    @SuppressWarnings("unchecked")
    public <T> T getPlugin(Class<T> cls) {
        MengineApplication application = this.getMengineApplication();

        T plugin = application.getService(cls);

        return plugin;
    }

    public String getSessionId() {
        MengineApplication application = this.getMengineApplication();

        String sessionId = application.getSessionId();

        return sessionId;
    }

    public String getVersionName() {
        MengineApplication application = this.getMengineApplication();

        String versionName = application.getVersionName();

        return versionName;
    }

    public void setState(String name, Object value) {
        MengineApplication application = this.getMengineApplication();

        application.setState(name, value);
    }

    public boolean hasMetaData(String name) {
        MengineApplication application = this.getMengineApplication();

        boolean result = application.hasMetaData(name);

        return result;
    }

    public String getMetaDataString(String name) {
        MengineApplication application = this.getMengineApplication();

        String value = application.getMetaDataString(name);

        return value;
    }

    public boolean getMetaDataBoolean(String name) {
        MengineApplication application = this.getMengineApplication();

        boolean value = application.getMetaDataBoolean(name);

        return value;
    }

    public int getMetaDataInteger(String name) {
        MengineApplication application = this.getMengineApplication();

        int value = application.getMetaDataInteger(name);

        return value;
    }

    public ViewGroup getContentViewGroup() {
        ViewGroup viewGroup = m_contentView;

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

        MengineTransparencyConsentParam tcParam = application.makeTransparencyConsentParam();

        return tcParam;
    }

    public void onMengineTransparencyConsent(MengineTransparencyConsentParam tcParam) {
        MengineApplication application = this.getMengineApplication();

        application.onMengineTransparencyConsent(tcParam);
    }

    public void checkPermission(String permission, Runnable onSuccess, Runnable onFailure) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.TIRAMISU) {
            onSuccess.run();

            return;
        }

        if (ContextCompat.checkSelfPermission(this, permission) == PackageManager.PERMISSION_GRANTED) {
            onSuccess.run();

            return;
        }

        ActivityResultLauncher<String> requestPermissionLauncher =
            this.registerForActivityResult(new ActivityResultContracts.RequestPermission(), isGranted -> {
                if (isGranted == true) {
                    onSuccess.run();
                } else {
                    onFailure.run();
                }
            });

        requestPermissionLauncher.launch(permission);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        MengineLog.logMessageRelease(TAG, "onCreate: %s", savedInstanceState);

        try {
            Thread.currentThread().setName("MengineActivity");
        } catch (final Exception e) {
            MengineLog.logMessage(TAG, "modify activity thread name exception: %s"
                , e.getMessage()
            );
        }

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            String invalidInitializeReason = application.getInvalidInitializeReason();

            MengineAnalytics.buildEvent("mng_activity_create_failed")
                .addParameterString("reason", invalidInitializeReason)
                .logAndFlush();

            this.finishWithAlertDialog(invalidInitializeReason);

            return;
        }

        application.setMengineActivity(this);

        this.setState("activity.lifecycle", "create");
        this.setState("activity.init", "begin");

        Looper mainLooper = Looper.getMainLooper();
        m_commandHandler = new MengineCommandHandler(mainLooper, this);
        m_semaphores = new HashMap<>();

        this.setState("activity.init", "setup_relativelayout");

        RelativeLayout contentView = new RelativeLayout(this);

        m_contentView = contentView;

        this.setState("activity.init", "setup_surface");

        MengineSurfaceView surface = new MengineSurfaceView(this);

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

        MengineNative.AndroidEnv_setMengineAndroidActivityJNI(this);

        MengineNative.AndroidNativePython_addPlugin("Activity", this);

        this.setState("activity.init", "plugin_create");

        List<MengineService> plugins = this.getPlugins();

        for (MengineService p : plugins) {
            p.setMengineActivity(this);
        }

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logMessage(TAG, "onCreate plugin: %s"
                , l.getServiceName()
            );

            try {
                l.onCreate(this, savedInstanceState);
            } catch (final MengineServiceInvalidInitializeException e) {
                this.setState("activity.init", "plugin_create_exception." + l.getServiceName());

                MengineAnalytics.buildEvent("mng_activity_create_failed")
                    .addParameterException("reason", e)
                    .logAndFlush();

                this.finishWithAlertDialog("[ERROR] create plugin: %s exception: %s"
                    , l.getServiceName()
                    , e.getMessage()
                );

                return;
            }
        }

        this.setState("activity.init", "end");

        this.setState("activity.lifecycle", "created");
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onPostCreate: application invalid initialize");

            return;
        }

        this.setState("activity.lifecycle", "post_create");

        MengineLog.logMessage(TAG, "onPostCreate");
    }

    @SuppressWarnings("deprecation")
    private void setupFullscreenModeListener(Window window) {
        if (window == null) {
            MengineLog.logError(TAG, "setupFullscreenModeListener invalid window");

            return;
        }

        View decorView = window.getDecorView();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            decorView.setOnApplyWindowInsetsListener((v, insets) -> {
                boolean isSystemBarsVisible = insets.isVisible(WindowInsets.Type.systemBars());

                if (isSystemBarsVisible == true) {
                    MengineActivity.this.syncFullscreenWindow(window);
                }

                return insets;
            });
        } else {
            decorView.setOnSystemUiVisibilityChangeListener(visibility -> {
                boolean isFullscreenEnable = (visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) != 0;
                boolean isNavigationHidden = (visibility & View.SYSTEM_UI_FLAG_HIDE_NAVIGATION) != 0;

                if (isFullscreenEnable == false || isNavigationHidden == false) {
                    MengineActivity.this.syncFullscreenWindow(window);
                }
            });
        }
    }

    @SuppressWarnings("deprecation")
    private void releaseFullscreenModeListener(Window window) {
        if (window == null) {
            MengineLog.logError(TAG, "releaseFullscreenModeListener invalid window");

            return;
        }

        View decorView = window.getDecorView();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            decorView.setOnApplyWindowInsetsListener(null);
        } else {
            decorView.setOnSystemUiVisibilityChangeListener(null);
        }
    }

    @SuppressWarnings("deprecation")
    private void syncFullscreenWindowR(Window window) {
        int flags = View.SYSTEM_UI_FLAG_FULLSCREEN |
                View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
                View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE;

        View decorView = window.getDecorView();
        decorView.setSystemUiVisibility(flags);

        window.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        window.addFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);
        window.clearFlags(WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN);
    }

    @SuppressWarnings("deprecation")
    private void syncFullscreenWindow(Window window) {
        if (window == null) {
            MengineLog.logError(TAG, "syncFullscreenWindow invalid window");

            return;
        }

        MengineLog.logMessage(TAG, "sync fullscreen mode");

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            window.setDecorFitsSystemWindows(false);

            WindowInsetsController insetsController = window.getInsetsController();

            if (insetsController == null) {
                this.syncFullscreenWindowR(window);
            } else {
                insetsController.hide(WindowInsets.Type.systemBars());
                insetsController.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
            }
        } else {
            this.syncFullscreenWindowR(window);
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            WindowManager.LayoutParams params = window.getAttributes();
            params.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            window.setAttributes(params);
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onWindowFocusChanged: application invalid initialize");

            return;
        }

        this.setState("activity.lifecycle", "window_focus_changed:" + (hasFocus == true ? "true" : "false"));

        MengineLog.logMessage(TAG, "onWindowFocusChanged focus: %s"
            , hasFocus
        );

        if (hasFocus == true) {
            Window window = this.getWindow();

            this.syncFullscreenWindow(window);
        }

        MengineNative.AndroidPlatform_windowFocusChangedEvent(hasFocus);
    }

    @Override
    public void onAttachedToWindow() {
        super.onAttachedToWindow();

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onAttachedToWindow: application invalid initialize");

            return;
        }

        this.setState("activity.lifecycle", "attached_to_window");

        MengineLog.logMessage(TAG, "onAttachedToWindow");

        Window window = this.getWindow();

        this.setupFullscreenModeListener(window);

        this.syncFullscreenWindowR(window);
    }

    @Override
    public void onDetachedFromWindow() {
        super.onDetachedFromWindow();

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onDetachedFromWindow: application invalid initialize");

            return;
        }

        this.setState("activity.lifecycle", "detached_from_window");

        MengineLog.logMessage(TAG, "onDetachedFromWindow");

        Window window = this.getWindow();

        this.releaseFullscreenModeListener(window);
    }

    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onSaveInstanceState: application invalid initialize");

            return;
        }

        this.setState("activity.lifecycle", "save_instance_state");

        MengineLog.logMessage(TAG, "onSaveInstanceState");
    }

    @Override
    public void onRestoreInstanceState(@NonNull Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onRestoreInstanceState: application invalid initialize");

            return;
        }

        this.setState("activity.lifecycle", "restore_instance_state");

        MengineLog.logMessage(TAG, "onRestoreInstanceState: %s"
            , savedInstanceState
        );
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
    	this.setState("activity.result", "request: " + requestCode + "result: " + resultCode );

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onActivityResult: application invalid initialize");

            return;
        }

        MengineLog.logMessageRelease(TAG, "onActivityResult request: %d result: %d"
            , requestCode
            , resultCode
        );

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onActivityResultBefore(this, requestCode, resultCode, data);
        }

        super.onActivityResult(requestCode, resultCode, data);

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onActivityResult(this, requestCode, resultCode, data);
        }
    }

    @Override
    public void onStart() {
        super.onStart();

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onStart: application invalid initialize");

            return;
        }

        this.setState("activity.lifecycle", "start");

        MengineLog.logMessageRelease(TAG, "onStart");

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onStart(this);
        }

        this.setState("activity.lifecycle", "started");

        MengineNative.AndroidPlatform_startEvent();
    }

    @Override
    public void onStop() {
        super.onStop();

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onStop: application invalid initialize");

            return;
        }

        this.setState("activity.lifecycle", "stop");

        MengineLog.logMessageRelease(TAG, "onStop");

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onStop(this);
        }

        this.setState("activity.lifecycle", "stoped");

        MengineNative.AndroidPlatform_stopEvent();
    }

    @Override
    public void onPause() {
        super.onPause();

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onPause: application invalid initialize");

            return;
        }

        this.setState("activity.lifecycle", "pause");

        MengineLog.logMessageRelease(TAG, "onPause");

        if (m_surfaceView != null) {
            m_surfaceView.handlePause();
        }

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onPause(this);
        }

        this.setState("activity.lifecycle", "paused");

        MengineNative.AndroidPlatform_pauseEvent();
    }

    @Override
    public void onResume() {
        super.onResume();

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onResume: application invalid initialize");

            return;
        }

        this.setState("activity.lifecycle", "resume");

        MengineLog.logMessageRelease(TAG, "onResume");

        if (m_surfaceView != null) {
            m_surfaceView.handleResume();
        }

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onResume(this);
        }

        this.setState("activity.lifecycle", "resumed");

        MengineNative.AndroidPlatform_resumeEvent();
    }

    @Override
    public void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onNewIntent: application invalid initialize");

            return;
        }

        this.setState("activity.intent_action", intent.getAction() );

        MengineLog.logMessageRelease(TAG, "onNewIntent intent: %s", intent);

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onNewIntent(this, intent);
        }
    }

    @Override
    public void onDestroy() {
        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onDestroy: application invalid initialize");

            super.onDestroy();

            return;
        }

        this.setState("activity.lifecycle", "destroy");

        MengineLog.logMessageRelease(TAG, "onDestroy");

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onDestroy(this);
        }

        List<MengineService> plugins = this.getPlugins();

        for (MengineService p : plugins) {
            p.setMengineActivity(null);
        }

        if (m_clipboard != null) {
            m_clipboard.handleDestroy();
            m_clipboard = null;
        }

        if (m_surfaceView != null) {
            m_surfaceView.handleDestroy();
            m_surfaceView = null;
        }

        if (m_softInput != null) {
            m_softInput.handleDestroy();
            m_softInput = null;
        }

        if (m_contentView != null) {
            m_contentView.removeAllViews();
            m_contentView = null;
        }

        m_semaphores = null;
        m_commandHandler = null;

        MengineNative.AndroidNativePython_removePlugin("Activity");

        MengineNative.AndroidEnv_removeMengineAndroidActivityJNI();

        application.setMengineActivity(null);

        super.onDestroy();
    }

    @Override
    public void onRestart() {
        super.onRestart();

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onRestart: application invalid initialize");

            return;
        }

        this.setState("activity.lifecycle", "restart");

        MengineLog.logMessage(TAG, "onRestart");

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
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

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onLowMemory: application invalid initialize");

            return;
        }

        this.setState("activity.low_memory", true);

        MengineLog.logMessage(TAG, "onLowMemory");

        MengineNative.AndroidPlatform_lowMemory();
    }

    @Override
    public void onTrimMemory(int level) {
        super.onTrimMemory(level);

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onTrimMemory: application invalid initialize");

            return;
        }

        this.setState("activity.trim_memory", level);

        MengineLog.logMessage(TAG, "onTrimMemory level: %d"
            , level
        );

        MengineNative.AndroidPlatform_trimMemory(level);
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onConfigurationChanged: application invalid initialize");

            return;
        }

        this.setState("configuration.orientation", newConfig.orientation);

        MengineLog.logMessage(TAG, "onConfigurationChanged config: %s"
            , newConfig.toString()
        );

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onConfigurationChanged(this, newConfig);
        }

        Locale newLocale = MengineUtils.getConfigurationLocale(newConfig);

        if (m_currentLocale == null || m_currentLocale.equals(newLocale) == false) {
            m_currentLocale = newLocale;

            String language = m_currentLocale.getLanguage();

            MengineNative.AndroidPlatform_changeLocale(language);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onRequestPermissionsResult: application invalid initialize");

            return;
        }

        MengineLog.logMessage(TAG, "onRequestPermissionsResult request: %d permissions: %s grantResults: %s"
            , requestCode
            , Arrays.toString(permissions)
            , Arrays.toString(grantResults)
        );

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            l.onRequestPermissionsResult(this, requestCode, permissions, grantResults);
        }
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "dispatchKeyEvent: application invalid initialize");

            return super.dispatchKeyEvent(event);
        }

        MengineLog.logInfo(TAG, "dispatchKeyEvent action: %d key: %d scan: %d"
            , event.getAction()
            , event.getKeyCode()
            , event.getScanCode()
        );

        List<MengineListenerKeyEvent> listeners = this.getKeyListeners();

        for (MengineListenerKeyEvent l : listeners) {
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

    public void pythonCall(String plugin, String method, Object ... args) {
        if (BuildConfig.DEBUG == true) {
            MengineLog.logInfo(TAG, "pythonCall plugin [%s] method [%s] args [%s]"
                , plugin
                , method
                , Arrays.toString(args)
            );
        }

        this.setState("python.call", plugin + "." + method);

        MengineNative.AndroidNativePython_call(plugin, method, args);
    }

    /***********************************************************************************************
     * Semaphore Methods
     **********************************************************************************************/

    public void activateSemaphore(String name) {
        MengineLog.logMessage(TAG, "activateSemaphore semaphore: %s"
            , name
        );

        this.setState("python.semaphore", name);

        ArrayList<MengineSemaphoreListener> activate_listeners;

        synchronized (m_syncronizationSemaphores) {
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
        MengineLog.logMessage(TAG, "deactivateSemaphore semaphore: %s"
            , name
        );

        synchronized (m_syncronizationSemaphores) {
            m_semaphores.remove(name);
        }
    }

    public void waitSemaphore(String name, MengineSemaphoreListener cb) {
        MengineLog.logMessage(TAG, "waitSemaphore semaphore: %s"
            , name
        );

        synchronized (m_syncronizationSemaphores) {
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

            boolean hasAccount = MengineNative.AndroidEnvironmentService_hasCurrentAccount();

            if (hasAccount == true) {
                String extraPreferencesFolderName = MengineNative.AndroidEnvironmentService_getExtraPreferencesFolderName();
                String accountFolderName = MengineNative.AndroidEnvironmentService_getCurrentAccountFolderName();

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

            if (MengineNative.AndroidEnvironmentService_writeCurrentLogToFile(logFileStream) == true) {
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

            if (MengineNative.AndroidEnvironmentService_writeOldLogToFile(oldLogFileStream) == true) {
                oldLogFileStream.write("\n\n[END OLD LOG]");
                oldLogFileStream.flush();
                oldLogFileStream.close();

                File oldLogZipFile = MengineUtils.createTempFile(context, "mng_old_log_", ".zip");

                if (oldLogZipFile != null && MengineUtils.zipFiles(oldLogFile, oldLogZipFile) == true) {
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

    public boolean linkingOpenDeleteAccount() {
        MengineLog.logMessage(TAG, "request delete account");

        MengineUtils.showAreYouSureAlertDialog(this
            , () -> { //Yes
                MengineLog.logMessage(TAG, "delete account [YES]");

                MengineApplication application = (MengineApplication)this.getApplication();

                application.removeSessionData();

                MengineNative.AndroidEnvironmentService_deleteCurrentAccount();

                MengineUtils.showOkAlertDialog(this, () -> {
                    this.finishAndRemoveTask();
                }, "Account Deleted", "Account data has been deleted. The application will now close.");
            }
            , () -> { //Cancel
                MengineLog.logMessage(TAG, "delete account [CANCEL]");
            }
            , 3000
            , "Delete Account"
            , "Click 'YES' will delete all account data. All game progress, virtual goods, and currency will be permanently removed and unrecoverable."
        );

        return true;
    }
}
