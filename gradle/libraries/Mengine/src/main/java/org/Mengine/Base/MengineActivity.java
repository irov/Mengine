package org.Mengine.Base;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

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
import java.util.Set;

public class MengineActivity extends SDLActivity {
    public static final String TAG = "MengineActivity";

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
    private static native void AndroidNativePython_call(String plugin, String method, Object args[]);

    private boolean m_initializePython;
    private boolean m_destroy;

    private Map<String, Integer> m_requestCodes;
    private Map<String, MengineSemaphore> m_semaphores;

    //private boolean mBrokenLibraries = false;

    //RelativeLayout m_contentView;

    public MengineActivity() {
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "SDLApplication"
        };
    }

    @Override
    protected String getMainSharedObject() {
        String mainSharedObject = this.getApplicationInfo().nativeLibraryDir + "/" + "libSDLApplication.so";

        return mainSharedObject;
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
        View view = SDLActivity.getContentView();

        ViewGroup viewGroup = (ViewGroup)view;

        return viewGroup;
    }

    /*
    public ViewGroup getContentViewGroup() {
        ViewGroup viewGroup = (ViewGroup)m_contentView;

        return viewGroup;
    }*/

    /*
    public static Context getContext() {
        Context context = SDLActivity.getContext();

        return context;
    }
     */

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
        m_initializePython = false;
        m_destroy = false;

        m_requestCodes = new HashMap<>();
        m_semaphores = new HashMap<>();

        /*
        RelativeLayout contentView = new RelativeLayout(this);
        this.setContentView(contentView);
        m_contentView = contentView;
         */

        this.setState("activity.lifecycle", "create");

        this.setState("activity.init", "start");

        try {
            super.onCreate(savedInstanceState);
        } catch (Exception e) {
            this.setState("activity.init", "sdl_exception");

            MengineAnalytics.buildEvent("mng_activity_init_failed")
                .addParameterException("reason", e)
                .logAndFlush();

            this.finishWithAlertDialog("[ERROR] SDL create exception: %s"
                , e.getMessage()
            );

            return;
        }

        if (mBrokenLibraries == true) {
            this.setState("activity.init", "sdl_broken_libraries");

            MengineAnalytics.buildEvent("mng_activity_init_failed")
                .addParameterString("reason", "sdl broken libraries")
                .logAndFlush();

            this.finishWithAlertDialog("[ERROR] SDL broken libraries");

            return;
        }

        AndroidEnv_setMengineAndroidActivityJNI(this);

        MengineApplication application = (MengineApplication)this.getApplication();

        if (application.isInvalidInitialize() == true) {
            this.finishWithAlertDialog("[ERROR] Application invalid initialize");

            return;
        }

        this.setState("activity.init", "create");

        MengineLog.logMessage(TAG, "onCreate");

        List<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.setActivity(this);
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            String pluginName = l.getPluginName();

            MengineLog.logMessage(TAG, "onCreate plugin: %s"
                , pluginName
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

        this.setState("activity.init", "completed");

        this.setState("activity.lifecycle", "created");

        /*
        MengineGLSurfaceView surface = new MengineGLSurfaceView(this);

        MengineGLSurfaceRenderer renderer = new MengineGLSurfaceRenderer();
        surface.setRenderer(renderer);

        m_contentView.addView(surface);

        Window window = this.getWindow();

        int flags = View.SYSTEM_UI_FLAG_FULLSCREEN |
                    View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                    View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
                    View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                    View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.INVISIBLE;
                window.getDecorView().setSystemUiVisibility(flags);
                window.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
                window.clearFlags(WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN);
         */
    }

    public void quitMengineApplication() {
        AndroidEnvironmentService_quitMengineAndroidActivityJNI();
    }

    public void onMengineInitializeBaseServices() {
        MengineLog.logInfo(TAG, "onMengineInitializeBaseServices");

        MengineLog.initialize(this);

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            l.onMengineInitializeBaseServices(this);
        }
    }

    public void onMengineCreateApplication() {
        MengineLog.logInfo(TAG, "onMengineCreateApplication");

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            l.onMengineCreateApplication(this);
        }
    }

    public void onMenginePlatformRun() {
        MengineLog.logInfo(TAG, "onMenginePlatformRun");

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            l.onMenginePlatformRun(this);
        }
    }

    public void onMenginePlatformStop() {
        MengineLog.logInfo(TAG, "onMenginePlatformStop");

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
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
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        MengineLog.logMessage(TAG, "onActivityResult request: %d result: %d"
            , requestCode
            , resultCode
        );

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onStart: broken libraries");

            super.onActivityResult(requestCode, resultCode, data);

            return;
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            l.onActivityResultBefore(this, requestCode, resultCode, data);
        }

        super.onActivityResult(requestCode, resultCode, data);

        for (MenginePluginActivityListener l : listeners) {
            l.onActivityResult(this, requestCode, resultCode, data);
        }
    }

    @Override
    protected void onStart() {
        super.onStart();

        this.setState("activity.lifecycle", "start");

        MengineLog.logMessage(TAG, "onStart");

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onStart: broken libraries");

            return;
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            l.onStart(this);
        }

        this.setState("activity.lifecycle", "started");
    }

    @Override
    protected void onStop() {
        super.onStop();

        this.setState("activity.lifecycle", "stop");

        MengineLog.logMessage(TAG, "onStop");

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onStop: broken libraries");

            return;
        }

        List<MenginePlugin> plugins = this.getPlugins();

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            l.onStop(this);
        }

        this.setState("activity.lifecycle", "stoped");
    }

    @Override
    protected void onPause() {
        super.onPause();

        this.setState("activity.lifecycle", "pause");

        MengineLog.logMessage(TAG, "onPause");

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onPause: broken libraries");

            return;
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            l.onPause(this);
        }

        this.setState("activity.lifecycle", "paused");
    }

    @Override
    protected void onResume() {
        super.onResume();

        this.setState("activity.lifecycle", "resume");

        MengineLog.logMessage(TAG, "onResume");

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onResume: broken libraries");

            return;
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            l.onResume(this);
        }

        this.setState("activity.lifecycle", "resumed");
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        this.setState("activity.intent_action", intent.getAction() );

        MengineLog.logMessage(TAG, "onNewIntent intent: %s", intent);

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onNewIntent: broken libraries");

            return;
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            l.onNewIntent(this, intent);
        }
    }

    @Override
    protected void onDestroy() {
        m_destroy = true;

        this.setState("activity.lifecycle", "destroy");

        MengineLog.logMessage(TAG, "onDestroy");

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onDestroy: broken libraries");

            super.onDestroy();

            return;
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            l.onDestroy(this);
        }

        List<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.setActivity(null);
        }

        m_requestCodes = null;
        m_semaphores = null;

        AndroidEnv_removeMengineAndroidActivityJNI();

        super.onDestroy();
    }

    @Override
    protected void onRestart() {
        super.onRestart();

        this.setState("activity.lifecycle", "restart");

        MengineLog.logMessage(TAG, "onRestart");

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onRestart: broken libraries");

            return;
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            l.onRestart(this);
        }

        this.setState("activity.lifecycle", "restarted");
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        this.setState("configuration.orientation", newConfig.orientation);

        MengineLog.logMessage(TAG, "onConfigurationChanged config: %s"
            , newConfig.toString()
        );

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onConfigurationChanged: broken libraries");

            return;
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
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

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onRequestPermissionsResult: broken libraries");

            return;
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
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

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "dispatchKeyEvent: broken libraries");

            return super.dispatchKeyEvent(event);
        }

        List<MenginePluginKeyListener> listeners = this.getKeyListeners();

        for (MenginePluginKeyListener l : listeners) {
            if (l.dispatchKeyEvent(this, event) == true) {
                return true;
            }
        }

        return super.dispatchKeyEvent(event);
    }

    @Override
    protected SDLSurface createSDLSurface(Context context) {
        return new MengineSurface(context);
    }

    /***********************************************************************************************
     * Kernel Methods
     **********************************************************************************************/

    public int genRequestCode(String name) {
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
        intent.putExtra(Intent.EXTRA_EMAIL, new String[]{email});
        intent.putExtra(Intent.EXTRA_SUBJECT, subject);
        intent.putExtra(Intent.EXTRA_TEXT, body);

        boolean hasAccount = AndroidEnvironmentService_hasCurrentAccount();

        if (hasAccount == true) {
            try {
                intent.setType("message/rfc822");

                intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
                intent.addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION);

                ArrayList<Uri> fileUris = new ArrayList<Uri>();

                File filesDir = context.getFilesDir();

                String extraPreferencesFolderName = AndroidEnvironmentService_getExtraPreferencesFolderName();
                String accountFolderName = AndroidEnvironmentService_getCurrentAccountFolderName();

                File extraPreferencesFolder = new File(filesDir, extraPreferencesFolderName);
                File accountFolder = new File(extraPreferencesFolder, accountFolderName);

                File accountZipFile = MengineUtils.createTempFile(context, "mng_account_", ".zip");

                if (accountZipFile == null) {
                    MengineLog.logWarning(TAG, "linkingOpenMail invalid create temp file 'mng_account_***.zip' mail: %s subject: %s"
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

                    MengineLog.logInfo(TAG, "linkingOpenMail attach file '%s' mail: %s subject: %s"
                        , accountZIPUri
                        , email
                        , subject
                    );

                    fileUris.add(accountZIPUri);
                }

                File logFile = MengineUtils.createTempFile(context, "mng_log_", ".log");

                if (logFile == null) {
                    MengineLog.logWarning(TAG, "linkingOpenMail invalid create temp file 'mng_log_***.log' mail: %s subject: %s"
                        , email
                        , subject
                    );

                    return false;
                }

                OutputStreamWriter logFileStream = new OutputStreamWriter(new FileOutputStream(logFile));

                if (AndroidEnvironmentService_writeCurrentLogToFile(logFileStream) == true) {
                    logFileStream.flush();
                    logFileStream.close();

                    File logZipFile = MengineUtils.createTempFile(context, "mng_log_", ".zip");

                    if (MengineUtils.zipFiles(logFile, logZipFile) == true) {
                        Uri logZipFileUri = MengineUtils.getUriForFile(context, logZipFile);

                        if (logZipFileUri == null) {
                            return false;
                        }

                        MengineLog.logInfo(TAG, "linkingOpenMail attach file '%s' mail: %s subject: %s"
                            , logZipFileUri
                            , email
                            , subject
                        );

                        fileUris.add(logZipFileUri);
                    }
                }

                File oldLogFile = MengineUtils.createTempFile(context, "mng_old_log_", ".log");

                if (oldLogFile == null) {
                    MengineLog.logWarning(TAG, "linkingOpenMail invalid create temp file 'mng_old_log_***.log' mail: %s subject: %s"
                        , email
                        , subject
                    );

                    return false;
                }

                OutputStreamWriter oldLogFileStream = new OutputStreamWriter(new FileOutputStream(oldLogFile));

                if (AndroidEnvironmentService_writeOldLogToFile(oldLogFileStream) == true) {
                    oldLogFileStream.flush();
                    oldLogFileStream.close();

                    File oldLogZipFile = MengineUtils.createTempFile(context, "mng_old_log_", ".zip");

                    if (MengineUtils.zipFiles(oldLogFile, oldLogZipFile) == true) {
                        Uri oldLogZipFileUri = MengineUtils.getUriForFile(context, oldLogZipFile);

                        if (oldLogZipFileUri == null) {
                            return false;
                        }

                        MengineLog.logInfo(TAG, "linkingOpenMail attach file '%s' mail: %s subject: %s"
                            , oldLogZipFileUri
                            , email
                            , subject
                        );

                        fileUris.add(oldLogZipFileUri);
                    }
                }

                intent.putParcelableArrayListExtra(Intent.EXTRA_STREAM, fileUris);
            } catch (IOException e) {
                MengineLog.logError(TAG, "[ERROR] linkingOpenMail failed attachs file mail: %s subject: %s exception: %s"
                    , email
                    , subject
                    , e.getMessage()
                );
            }
        } else {
            intent.setType("text/plain");
        }

        Intent chooser = Intent.createChooser(intent, "Send Email");

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
