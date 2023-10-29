package org.Mengine.Base;

import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.List;

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
    private static native boolean AndroidEnvironmentService_writeLoggerHistoryToFile(Writer writer);
    private static native boolean AndroidEnvironmentService_writeOldLogToFile(Writer writer);
    private static native int AndroidEnvironmentService_getProjectVersion();

    private static native void AndroidNativePython_addPlugin(String name, Object plugin);
    private static native void AndroidNativePython_call(String plugin, String method, Object args[]);
    private static native void AndroidNativePython_activateSemaphore(String name);

    private boolean m_initializeBaseServices;
    private boolean m_initializePython;
    private boolean m_destroy;

    private Map<String, MengineSemaphore> m_semaphores;

    private Map<String, Integer> m_requestCodes;

    public MengineActivity() {
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
            "SDL2"
        };
    }

    @Override
    protected String getMainSharedObject() {
        String mainSharedObject = getContext().getApplicationInfo().nativeLibraryDir + "/" + "libSDLApplication.so";

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
        MengineApplication app = (MengineApplication)this.getApplication();

        return app;
    }

    public String getApplicationId() {
        MengineApplication app = this.getMengineApplication();

        String applicationId = app.getApplicationId();

        return applicationId;
    }

    protected List<MenginePlugin> getPlugins() {
        MengineApplication app = this.getMengineApplication();

        List<MenginePlugin> plugins = app.getPlugins();

        return plugins;
    }

    protected List<MenginePluginKeyListener> getKeyListeners() {
        MengineApplication app = this.getMengineApplication();

        List<MenginePluginKeyListener> listeners = app.getKeyListeners();

        return listeners;
    }

    protected List<MenginePluginActivityListener> getActivityListeners() {
        MengineApplication app = this.getMengineApplication();

        List<MenginePluginActivityListener> listeners = app.getActivityListeners();

        return listeners;
    }

    protected List<MenginePluginEngineListener> getEngineListeners() {
        MengineApplication app = this.getMengineApplication();

        List<MenginePluginEngineListener> listeners = app.getEngineListeners();

        return listeners;
    }

    @SuppressWarnings("unchecked")
    public <T> T getPlugin(Class<T> cls) {
        if (m_destroy == true) {
            return null;
        }

        MengineApplication app = this.getMengineApplication();

        T plugin = app.getPlugin(cls);

        return plugin;
    }

    public String getSessionId() {
        if (m_destroy == true) {
            return "";
        }

        MengineApplication app = this.getMengineApplication();

        String sessionId = app.getSessionId();

        return sessionId;
    }

    public void setState(String name, Object value) {
        if (m_destroy == true) {
            return;
        }

        MengineApplication app = this.getMengineApplication();

        app.setState(name, value);
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

    public boolean getMetaDataBoolean(String name, boolean d) {
        if (m_destroy == true) {
            return d;
        }

        MengineApplication application = this.getMengineApplication();

        boolean value = application.getMetaDataBoolean(name, d);

        return value;
    }

    public int getMetaDataInteger(String name, int d) {
        if (m_destroy == true) {
            return d;
        }

        MengineApplication application = this.getMengineApplication();

        int value = application.getMetaDataInteger(name, d);

        return value;
    }

    public static View getContentView() {
        View view = SDLActivity.getContentView();

        return view;
    }

    public static Context getContext() {
        Context context = SDLActivity.getContext();

        return context;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        m_initializeBaseServices = false;
        m_initializePython = false;
        m_destroy = false;

        m_semaphores = new HashMap<>();
        m_requestCodes = new HashMap<>();

        this.setState("activity.lifecycle", "create");

        this.setState("activity.init", "start");

        long activity_init_start_timestamp = MengineAnalytics.buildEvent("mng_activity_init_start")
            .logAndFlush();

        super.onCreate(savedInstanceState);

        this.setState("activity.init", "create");

        MengineLog.logMessage(TAG, "onCreate");

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onCreate: broken libraries");

            MengineAnalytics.buildEvent("mng_activity_init_broken_libraries")
                .logAndFlush();

            return;
        }

        AndroidEnv_setMengineAndroidActivityJNI(this);

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

            long plugin_init_plugin_start_timestamp = MengineAnalytics.buildEvent("mng_activity_init_plugin_start")
                .addParameterString("name", pluginName)
                .logAndFlush();

            try {
                l.onCreate(this, savedInstanceState);
            } catch (MenginePluginInvalidInitializeException e) {
                MengineLog.logError(TAG, "[ERROR] onCreate plugin: %s exception: %s"
                    , l.getPluginName()
                    , e.getLocalizedMessage()
                );

                this.finish();
            }

            MengineAnalytics.buildEvent("mng_activity_init_plugin_completed")
                .addParameterString("name", pluginName)
                .addParameterLong("time", MengineUtils.getDurationTimestamp(plugin_init_plugin_start_timestamp))
                .logAndFlush();
        }

        this.setState("activity.init", "plugin_create");

        for (MenginePlugin p : plugins) {
            try {
                MengineLog.logMessage(TAG, "onExtensionInitialize plugin: %s"
                    , p.getPluginName()
                );

                p.onExtensionInitialize(this);
            } catch (MenginePluginInvalidInitializeException e) {
                MengineLog.logError(TAG, "[ERROR] onExtensionInitialize plugin %s: exception: %s"
                    , p.getPluginName()
                    , e.getLocalizedMessage()
                );

                this.finish();
            }
        }

        this.setState("activity.init", "completed");

        MengineAnalytics.buildEvent("mng_activity_init_completed")
            .addParameterLong("time", MengineUtils.getDurationTimestamp(activity_init_start_timestamp))
            .logAndFlush();

        this.setState("activity.lifecycle", "created");
    }

    public void quitMengineApplication() {
        AndroidEnvironmentService_quitMengineAndroidActivityJNI();
    }

    public void onMengineInitializeBaseServices() {
        m_initializeBaseServices = true;

        MengineLog.onMengineInitializeBaseServices(this);

        MengineLog.logInfo(TAG, "onMengineInitializeBaseServices");

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

        List<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onExtensionRun(this);
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

        MengineLog.onMenginePlatformStop(this);

        m_initializeBaseServices = false;
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
            l.onActivityResult(this, requestCode, resultCode, data);
        }

        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onStart(){
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

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onStart");
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

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onStop");
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

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onPause");
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

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onResume");
        }

        this.setState("activity.lifecycle", "resumed");
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

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
        this.setState("activity.lifecycle", "destroy");

        MengineLog.logMessage(TAG, "onDestroy");

        if (mBrokenLibraries == true) {
            MengineLog.logWarning(TAG, "onDestroy: broken libraries");

            super.onDestroy();

            return;
        }

        m_destroy = true;

        List<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onExtensionFinalize(this);
        }

        List<MenginePluginActivityListener> listeners = this.getActivityListeners();

        for (MenginePluginActivityListener l : listeners) {
            l.onDestroy(this);
        }

        for (MenginePlugin p : plugins) {
            p.setActivity(null);
        }

        m_semaphores = null;
        m_requestCodes = null;

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

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onRestart");
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

    public void pythonInitializePlugins() {
        MengineLog.logInfo(TAG, "pythonInitializePlugins");

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

    public void pythonFinalizePlugins() {
        m_initializePython = false;
    }

    public void pythonCall(String plugin, String method, Object ... args) {
        if (m_initializePython == false) {
            MengineLog.logWarning(TAG,"pythonCall call before embedding plugin: %s method: %s args: %s"
                , plugin
                , method
                , args
            );

            return;
        }

        if (BuildConfig.DEBUG == true) {
            MengineLog.logInfo(TAG, "pythonCall plugin [%s] method [%s] args [%s]"
                , plugin
                , method
                , args
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

        MengineSemaphore semaphore = m_semaphores.get(name);

        if (semaphore == null) {
            MengineSemaphore new_semaphore = new MengineSemaphore(true);

            m_semaphores.put(name, new_semaphore);

            return;
        }

        semaphore.activate();
    }

    public void waitSemaphore(String name, MengineFunctorVoid cb) {
        MengineSemaphore semaphore = m_semaphores.get(name);

        if (semaphore == null) {
            MengineSemaphore new_semaphore = new MengineSemaphore(false);

            new_semaphore.addListener(cb);

            m_semaphores.put(name, new_semaphore);

            return;
        }

        semaphore.addListener(cb);
    }

    public void waitAndroidSemaphore(String name) {
        MengineLog.logMessage(TAG, "waitAndroidSemaphore semaphore: %s"
            , name
        );

        MengineFunctorVoid cb = () -> {
            AndroidNativePython_activateSemaphore(name);
        };

        this.waitSemaphore(name, cb);
    }

    /***********************************************************************************************
     * Linking Methods
     **********************************************************************************************/

    public boolean linkingOpenURL(String url) {
        MengineLog.logMessage(TAG, "linkingOpenURL url: %s"
            , url
        );

        this.setState("open.url", url);

        Intent intent = new Intent(Intent.ACTION_VIEW);

        Uri uri = Uri.parse(url);
        intent.setData(uri);

        Intent chooser = Intent.createChooser(intent, "");

        try {
            this.startActivity(chooser);
        } catch (ActivityNotFoundException e) {
            MengineLog.logWarning(TAG, "linkingOpenURL url: %s catch ActivityNotFoundException: %s"
                , url
                , e.getLocalizedMessage()
            );

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

                if (AndroidEnvironmentService_writeLoggerHistoryToFile(logFileStream) == true) {
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
                    , e.getLocalizedMessage()
                );
            }
        } else {
            intent.setType("text/plain");
        }

        Intent chooser = Intent.createChooser(intent, "Send Email");
        this.startActivity(chooser);

        return true;
    }
}
