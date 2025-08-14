package org.Mengine.Base;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.os.Build;
import android.os.Bundle;
import android.os.Looper;
import android.view.KeyEvent;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.ActivityResultRegistry;
import androidx.activity.result.contract.ActivityResultContract;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Locale;
import java.util.Map;
import java.util.List;
import java.util.Objects;

public class MengineActivity extends AppCompatActivity {
    public static final MengineTag TAG = MengineTag.of("MActivity");

    public enum ELifecycleState {
        ACTIVITY_INITIALIZE,
        ACTIVITY_CREATE,
        ACTIVITY_POST_CREATE,
        ACTIVITY_START,
        ACTIVITY_RESUME,
        ACTIVITY_PAUSE,
        ACTIVITY_STOP,
        ACTIVITY_DESTROY,
        ACTIVITY_RESTART,
    }

    public static MengineActivity INSTANCE = null;

    private volatile ELifecycleState m_lifecycleState = ELifecycleState.ACTIVITY_INITIALIZE;

    private final List<MengineListenerActivity> m_activityListeners = new ArrayList<>();

    private Locale m_currentLocale;

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

    protected List<MengineListenerActivity> getActivityListeners() {
        return m_activityListeners;
    }

    @SuppressWarnings("unchecked")
    public <T> T getService(Class<T> cls) {
        MengineApplication application = MengineApplication.INSTANCE;

        T plugin = application.getService(cls);

        return plugin;
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

    protected void finishWithAlertDialog(String format, Object ... args) {
        if (BuildConfig.DEBUG == true) {
            MengineUI.finishActivityWithAlertDialog(this, "MengineActivity", format, args);
        } else {
            MengineUtils.performOnMainThreadDelayed(()-> {
                this.finishAndRemoveTask();
            }, 0L);
        }
    }

    public void checkPermission(String permission) {
        this.checkPermission(permission, null, null);
    }

    public void checkPermission(String permission, Runnable onSuccess, Runnable onFailure) {
        this.checkPermissionRationale(permission, onSuccess, onFailure, null, null);
    }

    public void checkPermissionRationale(String permission, Runnable onSuccess, Runnable onFailure, String title, String format, Object ... args) {
        if (ContextCompat.checkSelfPermission(this, permission) == PackageManager.PERMISSION_GRANTED) {
            MengineLog.logDebug(TAG, "checkPermission: %s already granted"
                , permission
            );

            if (onSuccess != null) {
                onSuccess.run();
            }

            return;
        }

        ActivityResultRegistry registry = this.getActivityResultRegistry();

        final String name = permission + MengineUtils.getRandomUUIDString();

        ActivityResultLauncher[] launcher = new ActivityResultLauncher[1];

        launcher[0] = registry.register(name
            , this
            , new ActivityResultContracts.RequestPermission()
            , isGranted -> {
                if (isGranted == true) {
                    MengineLog.logInfo(TAG, "checkPermission: %s granted"
                        , permission
                    );

                    launcher[0].unregister();

                    if (onSuccess != null) {
                        onSuccess.run();
                    }
                } else {
                    MengineLog.logInfo(TAG, "checkPermission: %s denied"
                        , permission
                    );

                    launcher[0].unregister();

                    if (onFailure != null) {
                        onFailure.run();
                    }
                }
            });

        if (title != null && format != null && this.shouldShowRequestPermissionRationale(permission) == true) {
            MengineLog.logInfo(TAG, "checkPermission: %s show rationale"
                , permission
            );

            MengineUI.showAllowPermissionAlertDialog(this
                , () -> {
                    MengineLog.logInfo(TAG, "checkPermission: %s show rationale [granted]"
                        , permission
                    );

                    launcher[0].launch(permission);
                }
                , () -> {
                    MengineLog.logInfo(TAG, "checkPermission: %s show rationale [denied]"
                        , permission
                    );

                    if (onFailure != null) {
                        onFailure.run();
                    }
                }
                , title
                , format
                , args
            );
        } else {
            MengineLog.logInfo(TAG, "checkPermission: %s request"
                , permission
            );

            launcher[0].launch(permission);
        }
    }

    public ActivityResultLauncher<Intent> registerForActivityResult(@NonNull ActivityResultCallback<ActivityResult> callback) {
        ActivityResultContract<Intent, ActivityResult> contract = new ActivityResultContracts.StartActivityForResult();

        ActivityResultLauncher<Intent> launcher = this.registerForActivityResult(contract, callback);

        return launcher;
    }

    public ELifecycleState getLifecycleState() {
        return m_lifecycleState;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        m_lifecycleState = ELifecycleState.ACTIVITY_CREATE;

        MengineLog.logDebug(TAG, "[BEGIN] onCreate savedInstanceState: %s"
            , savedInstanceState
        );

        long activity_timestamp = MengineUtils.getTimestamp();

        try {
            Thread.currentThread().setName("MengineActivity");
        } catch (final Exception e) {
            MengineLog.logMessage(TAG, "modify activity thread name exception: %s"
                , e.getMessage()
            );
        }

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            Throwable e = application.getInvalidInitializeException();

            MengineAnalytics.buildEvent("mng_activity_create_failed")
                .addParameterThrowable("reason", e)
                .logAndFlush();

            MengineLog.logException(TAG, e, Map.of());

            this.finishWithAlertDialog("Activity failed to initialize because the application encountered an error");

            return;
        }

        INSTANCE = this;

        List<MengineServiceInterface> services = application.getServices();

        for (MengineServiceInterface s : services) {
            s.onActivityInitialize(this);

            if (s instanceof MengineListenerActivity listener) {
                m_activityListeners.add(listener);
            }
        }

        application.setState("activity.lifecycle", "create");
        application.setState("activity.init", "begin");

        Resources resources = getResources();
        Configuration configuration = resources.getConfiguration();

        m_currentLocale = MengineUtils.getConfigurationLocale(configuration);

        Looper mainLooper = Looper.getMainLooper();
        m_commandHandler = new MengineCommandHandler(mainLooper, this);

        application.setState("activity.init", "setup_relativelayout");

        RelativeLayout contentView = new RelativeLayout(this);

        m_contentView = contentView;

        application.setState("activity.init", "setup_surface");

        MengineSurfaceView surface = new MengineSurfaceView(this);

        m_surfaceView = surface;

        m_contentView.addView(m_surfaceView);

        application.setState("activity.init", "setup_softinput");

        MengineSoftInput softInput = new MengineSoftInput(this, m_surfaceView);

        m_softInput = softInput;

        m_contentView.addView(m_softInput);

        application.setState("activity.init", "setup_contentview");

        this.setContentView(m_contentView);

        application.setState("activity.init", "setup_clipboard");

        MengineClipboard clipboard = new MengineClipboard(this);

        m_clipboard = clipboard;

        application.setState("activity.init", "bootstrap");

        MengineNative.AndroidKernelService_addPlugin("Activity", this);

        application.setState("activity.init", "plugin_create");

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            long service_timestamp = MengineUtils.getTimestamp();

            try {
                l.onCreate(this, savedInstanceState);
            } catch (final MengineServiceInvalidInitializeException e) {
                application.setState("activity.init", "plugin_create_exception." + l.getServiceName());

                MengineAnalytics.buildEvent("mng_activity_create_failed")
                    .addParameterException("reason", e)
                    .logAndFlush();

                MengineLog.logException(TAG, e, Map.of("plugin", l.getServiceName()));

                this.finishWithAlertDialog("Activity failed to initialize because the invalid create plugin %s"
                    , l.getServiceName()
                );

                return;
            }

            MengineLog.logDebug(TAG, "onCreate plugin: %s time: %d"
                , l.getServiceName()
                , MengineUtils.getTimestamp() - service_timestamp
            );
        }

        application.setState("activity.init", "end");

        application.setState("activity.lifecycle", "created");

        MengineLog.logDebug(TAG, "[END] onCreate time: %d"
            , MengineUtils.getTimestamp() - activity_timestamp
        );
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);

        m_lifecycleState = ELifecycleState.ACTIVITY_POST_CREATE;

        MengineLog.logDebug(TAG, "[BEGIN] onPostCreate savedInstanceState: %s"
            , savedInstanceState
        );

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onPostCreate: application invalid initialize");

            return;
        }

        application.setState("activity.lifecycle", "post_create");

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onPostCreate plugin: %s"
                , l.getServiceName()
            );

            try {
                l.onPostCreate(this, savedInstanceState);
            } catch (final MengineServiceInvalidInitializeException e) {
                application.setState("activity.init", "plugin_create_exception." + l.getServiceName());

                MengineAnalytics.buildEvent("mng_activity_create_failed")
                    .addParameterException("reason", e)
                    .logAndFlush();

                MengineLog.logException(TAG, e, Map.of("plugin", l.getServiceName()));

                this.finishWithAlertDialog("Activity failed to initialize because the invalid post create plugin %s"
                    , l.getServiceName()
                );

                return;
            }
        }

        MengineLog.logDebug(TAG, "[END] onPostCreate");
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
                    this.syncFullscreenWindow(window);
                }

                return insets;
            });
        } else {
            decorView.setOnSystemUiVisibilityChangeListener(visibility -> {
                boolean isFullscreenEnable = (visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) != 0;
                boolean isNavigationHidden = (visibility & View.SYSTEM_UI_FLAG_HIDE_NAVIGATION) != 0;

                if (isFullscreenEnable == false || isNavigationHidden == false) {
                    this.syncFullscreenWindow(window);
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
    private void syncFullscreenWindowQ(Window window) {
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

        MengineLog.logDebug(TAG, "sync fullscreen mode");

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            window.setDecorFitsSystemWindows(false);

            WindowInsetsController insetsController = window.getInsetsController();

            if (insetsController == null) {
                this.syncFullscreenWindowQ(window);
            } else {
                insetsController.hide(WindowInsets.Type.systemBars());
                insetsController.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
            }
        } else {
            this.syncFullscreenWindowQ(window);
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

        MengineLog.logDebug(TAG, "[BEGIN] onWindowFocusChanged focus: %s"
            , hasFocus
        );

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onWindowFocusChanged: application invalid initialize");

            return;
        }

        application.setState("activity.lifecycle", "window_focus_changed:" + (hasFocus == true ? "true" : "false"));

        if (hasFocus == true) {
            Window window = this.getWindow();

            this.syncFullscreenWindow(window);
        }

        MengineNative.AndroidPlatform_windowFocusChangedEvent(hasFocus);

        MengineLog.logDebug(TAG, "[END] onWindowFocusChanged focus: %s"
            , hasFocus
        );
    }

    @Override
    public void onAttachedToWindow() {
        super.onAttachedToWindow();

        MengineLog.logDebug(TAG, "[BEGIN] onAttachedToWindow");

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onAttachedToWindow: application invalid initialize");

            return;
        }

        application.setState("activity.lifecycle", "attached_to_window");

        Window window = this.getWindow();

        this.setupFullscreenModeListener(window);

        this.syncFullscreenWindow(window);

        MengineLog.logDebug(TAG, "[END] onAttachedToWindow");
    }

    @Override
    public void onDetachedFromWindow() {
        super.onDetachedFromWindow();

        MengineLog.logDebug(TAG, "[BEGIN] onDetachedFromWindow");

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onDetachedFromWindow: application invalid initialize");

            return;
        }

        application.setState("activity.lifecycle", "detached_from_window");

        Window window = this.getWindow();

        this.releaseFullscreenModeListener(window);

        MengineLog.logDebug(TAG, "[END] onDetachedFromWindow");
    }

    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);

        MengineLog.logDebug(TAG, "[BEGIN] onSaveInstanceState: %s"
            , outState
        );

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onSaveInstanceState: application invalid initialize");

            return;
        }

        application.setState("activity.lifecycle", "save_instance_state");

        MengineLog.logDebug(TAG, "[END] onSaveInstanceState");
    }

    @Override
    public void onRestoreInstanceState(@NonNull Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);

        MengineLog.logDebug(TAG, "[BEGIN] onRestoreInstanceState: %s"
            , savedInstanceState
        );

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onRestoreInstanceState: application invalid initialize");

            return;
        }

        application.setState("activity.lifecycle", "restore_instance_state");

        MengineLog.logDebug(TAG, "[END] onRestoreInstanceState");
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        MengineApplication application = (MengineApplication)this.getApplication();

        MengineLog.logDebug(TAG, "[BEGIN] onActivityResult request: %d result: %d"
            , requestCode
            , resultCode
        );

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onActivityResult: application invalid initialize");

            return;
        }

        application.setState("activity.result", "request." + requestCode + ".result." + resultCode );

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onActivityResultBefore plugin: %s request: %d result: %d"
                , l.getServiceName()
                , requestCode
                , resultCode
            );

            l.onActivityResultBefore(this, requestCode, resultCode, data);
        }

        super.onActivityResult(requestCode, resultCode, data);

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onActivityResult plugin: %s request: %d result: %d"
                , l.getServiceName()
                , requestCode
                , resultCode
            );

            l.onActivityResult(this, requestCode, resultCode, data);
        }

        MengineLog.logDebug(TAG, "[END] onActivityResult");
    }

    @Override
    public void onStart() {
        super.onStart();

        m_lifecycleState = ELifecycleState.ACTIVITY_START;

        MengineLog.logDebug(TAG, "[BEGIN] onStart");

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onStart: application invalid initialize");

            return;
        }

        application.setState("activity.lifecycle", "start");

        if (m_surfaceView != null) {
            m_surfaceView.handleStart();
        }

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onStart plugin: %s"
                , l.getServiceName()
            );

            l.onStart(this);
        }

        application.setState("activity.lifecycle", "started");

        MengineNative.AndroidPlatform_startEvent();

        MengineLog.logDebug(TAG, "[END] onStart");
    }

    @Override
    public void onStop() {
        super.onStop();

        m_lifecycleState = ELifecycleState.ACTIVITY_STOP;

        MengineLog.logDebug(TAG, "[BEGIN] onStop");

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onStop: application invalid initialize");

            return;
        }

        application.setState("activity.lifecycle", "stop");

        if (m_surfaceView != null) {
            m_surfaceView.handleStop();
        }

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onStop plugin: %s"
                , l.getServiceName()
            );

            l.onStop(this);
        }

        application.setState("activity.lifecycle", "stopped");

        MengineNative.AndroidPlatform_stopEvent();

        MengineLog.logDebug(TAG, "[END] onStop");
    }

    @Override
    public void onPause() {
        super.onPause();

        m_lifecycleState = ELifecycleState.ACTIVITY_PAUSE;

        MengineLog.logDebug(TAG, "[BEGIN] onPause");

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onPause: application invalid initialize");

            return;
        }

        application.setState("activity.lifecycle", "pause");

        application.onServicesSave();

        MengineStatistic.save(application);

        if (m_surfaceView != null) {
            m_surfaceView.handlePause();
        }

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onPause plugin: %s"
                , l.getServiceName()
            );

            l.onPause(this);
        }

        application.setState("activity.lifecycle", "paused");

        MengineNative.AndroidPlatform_pauseEvent();

        MengineLog.logDebug(TAG, "[END] onPause");
    }

    @Override
    public void onResume() {
        super.onResume();

        m_lifecycleState = ELifecycleState.ACTIVITY_RESUME;

        MengineLog.logDebug(TAG, "[BEGIN] onResume");

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onResume: application invalid initialize");

            return;
        }

        application.setState("activity.lifecycle", "resume");

        if (m_surfaceView != null) {
            m_surfaceView.handleResume();
        }

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onResume plugin: %s"
                , l.getServiceName()
            );

            l.onResume(this);
        }

        application.setState("activity.lifecycle", "resumed");

        MengineNative.AndroidPlatform_resumeEvent();

        MengineLog.logDebug(TAG, "[END] onResume");
    }

    @Override
    public void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        MengineLog.logDebug(TAG, "[BEGIN] onNewIntent intent: %s"
            , intent
        );

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onNewIntent: application invalid initialize");

            return;
        }

        application.setState("activity.intent_action", intent.getAction() );

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onNewIntent plugin: %s"
                , l.getServiceName()
            );

            l.onNewIntent(this, intent);
        }

        MengineLog.logDebug(TAG, "[END] onNewIntent intent");
    }

    @Override
    public void onDestroy() {
        m_lifecycleState = ELifecycleState.ACTIVITY_DESTROY;

        MengineLog.logDebug(TAG, "[BEGIN] onDestroy");

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onDestroy: application invalid initialize");

            super.onDestroy();

            return;
        }

        application.setState("activity.lifecycle", "destroy");

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onDestroy plugin: %s"
                , l.getServiceName()
            );

            l.onDestroy(this);
        }

        List<MengineServiceInterface> services = application.getServices();

        for (MengineServiceInterface s : services) {
            s.onActivityFinalize(this);
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

        m_commandHandler = null;

        MengineNative.AndroidKernelService_removePlugin("Activity");

        MengineNative.AndroidPlatform_destroyEvent();

        m_activityListeners.clear();

        MengineLog.logDebug(TAG, "[END] onDestroy");

        INSTANCE = null;

        super.onDestroy();
    }

    @Override
    public void onRestart() {
        super.onRestart();

        m_lifecycleState = ELifecycleState.ACTIVITY_RESTART;

        MengineLog.logDebug(TAG, "[BEGIN] onRestart");

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onRestart: application invalid initialize");

            return;
        }

        application.setState("activity.lifecycle", "restart");

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onRestart plugin: %s"
                , l.getServiceName()
            );

            l.onRestart(this);
        }

        application.setState("activity.lifecycle", "restarted");

        MengineNative.AndroidPlatform_restartEvent();

        MengineLog.logDebug(TAG, "[END] onRestart");
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        MengineLog.logDebug(TAG, "[BEGIN] onConfigurationChanged config: %s"
            , newConfig
        );

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onConfigurationChanged: application invalid initialize");

            return;
        }

        application.setState("configuration.orientation", newConfig.orientation);

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onConfigurationChanged plugin: %s"
                , l.getServiceName()
            );

            l.onConfigurationChanged(this, newConfig);
        }

        Locale newLocale = MengineUtils.getConfigurationLocale(newConfig);

        if (Objects.equals(m_currentLocale, newLocale) == false) {
            m_currentLocale = newLocale;

            String language = m_currentLocale.getLanguage();

            MengineNative.AndroidPlatform_changeLocale(language);
        }

        MengineLog.logDebug(TAG, "[END] onConfigurationChanged");
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        MengineLog.logDebug(TAG, "[BEGIN] onRequestPermissionsResult request: %d permissions: %s grantResults: %s"
            , requestCode
            , Arrays.toString(permissions)
            , Arrays.toString(grantResults)
        );

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "onRequestPermissionsResult: application invalid initialize");

            return;
        }

        List<MengineListenerActivity> listeners = this.getActivityListeners();

        for (MengineListenerActivity l : listeners) {
            if (l.onAvailable(application) == false) {
                continue;
            }

            MengineLog.logDebug(TAG, "onRequestPermissionsResult plugin: %s request: %d"
                , l.getServiceName()
                , requestCode
            );

            l.onRequestPermissionsResult(this, requestCode, permissions, grantResults);
        }

        MengineLog.logDebug(TAG, "[END] onRequestPermissionsResult");
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        MengineApplication application = (MengineApplication)this.getApplication();

        MengineLog.logDebug(TAG, "[BEGIN] dispatchKeyEvent action: %d key: %d scan: %d"
            , event.getAction()
            , event.getKeyCode()
            , event.getScanCode()
        );

        if (application.isInvalidInitialize() == true) {
            MengineLog.logMessage(TAG, "dispatchKeyEvent: application invalid initialize");

            return super.dispatchKeyEvent(event);
        }

        if (MengineFragmentKeyEvent.INSTANCE.dispatchKeyEvent(this, event) == true) {
            return true;
        }

        MengineLog.logDebug(TAG, "[END] dispatchKeyEvent");

        return super.dispatchKeyEvent(event);
    }

    public void postCommand(Runnable command) {
        m_commandHandler.post(command);
    }

    /***********************************************************************************************
     * Activity Listeners
     **********************************************************************************************/

    public void showAlertDialog(String caption, String message) {
        MengineUI.showOkAlertDialog(this, null, caption, message);
    }

    /***********************************************************************************************
     * Keyboard Methods
     **********************************************************************************************/

    public void showKeyboard() {
        MengineLog.logInfo(TAG, "showKeyboard");

        this.postCommand(() -> {
            m_softInput.showKeyboard();
        });
    }

    public void hideKeyboard() {
        MengineLog.logInfo(TAG, "hideKeyboard");

        this.postCommand(() -> {
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
        boolean result = MengineProcedureManager.execute(this, "openUrl", url);

        return result;
    }

    public boolean linkingOpenMail(String email, String subject, String body) {
        boolean result = MengineProcedureManager.execute(this, "sendMail", email, subject, body);

        return result;
    }

    public boolean linkingOpenDeleteAccount() {
        boolean result = MengineProcedureManager.execute(this, "deleteAccount");

        return result;
    }
}
