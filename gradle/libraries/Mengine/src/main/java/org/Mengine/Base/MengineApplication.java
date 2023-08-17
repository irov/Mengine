package org.Mengine.Base;

import android.app.AlertDialog;
import android.app.Application;
import android.content.ContentResolver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.multidex.MultiDex;

import org.libsdl.app.SDL;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

public class MengineApplication extends Application {
    private static final String TAG = "MengineApplication";

    private static native void AndroidEnv_setMengineAndroidApplicationJNI(Object activity);
    private static native void AndroidEnv_removeMengineAndroidApplicationJNI();

    private static native boolean AndroidEnv_isMasterRelease();
    private static native boolean AndroidEnv_isBuildPublish();
    private static native String AndroidEnv_getEngineGITSHA1();
    private static native String AndroidEnv_getBuildDate();
    private static native String AndroidEnv_getBuildUsername();

    public boolean isMasterRelease() {
        return AndroidEnv_isMasterRelease();
    }

    public boolean isBuildPublish() {
        return AndroidEnv_isBuildPublish();
    }

    public String getEngineGITSHA1() {
        return AndroidEnv_getEngineGITSHA1();
    }

    public String getBuildDate() {
        return AndroidEnv_getBuildDate();
    }

    public String getBuildUsername() {
        return AndroidEnv_getBuildUsername();
    }

    private String m_androidId;

    private String m_installKey;
    private long m_installTimestamp = -1;
    private String m_installDate;
    private long m_installRND = -1;
    private long m_sessionIndex = -1;
    private String m_sessionId;

    private ArrayList<MenginePlugin> m_plugins = new ArrayList<>();
    private Map<String, MenginePlugin> m_dictionaryPlugins = new HashMap<>();
    private Map<String, Object> m_states = new HashMap<>();

    private ArrayList<MenginePluginLoggerListener> m_loggerListeners = new ArrayList<>();
    private ArrayList<MenginePluginAnalyticsListener> m_analyticsListeners = new ArrayList<>();
    private ArrayList<MenginePluginActivityLifecycleListener> m_activityLifecycleListeners = new ArrayList<>();
    private ArrayList<MenginePluginKeyListener> m_keyListeners = new ArrayList<>();
    private ArrayList<MenginePluginApplicationListener> m_applicationListeners = new ArrayList<>();
    private ArrayList<MenginePluginActivityListener> m_activityListeners = new ArrayList<>();
    private ArrayList<MenginePluginExtensionListener> m_extensionListeners = new ArrayList<>();
    private ArrayList<MenginePluginEngineListener> m_engineListeners = new ArrayList<>();

    private MengineActivityLifecycle m_activityLifecycle;

    private final Object m_syncEvent = new Object();
    private final Object m_syncState = new Object();

    public String[] getGradleAndroidPlugins() {
        String[] plugins = {};

        return plugins;
    }

    public String getApplicationId() {
        return "";
    }

    public int getVersionCode() {
        return 0;
    }

    public String getVersionName() {
        return "";
    }

    public MengineApplication() {
        super();

        String[] plugins = this.getGradleAndroidPlugins();

        for (String namePlugin : plugins) {
            if (this.createPlugin(namePlugin) == false) {
                this.invalidInitialize("invalid create plugin: %s"
                    , namePlugin
                );
            }
        }
    }

    @SuppressWarnings("deprecation")
    private ApplicationInfo getPackageApplicationInfo(PackageManager packageManager, String packageName) throws PackageManager.NameNotFoundException {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.TIRAMISU) {
            PackageManager.ApplicationInfoFlags flags = PackageManager.ApplicationInfoFlags.of(PackageManager.GET_META_DATA);

            return packageManager.getApplicationInfo(packageName, flags);
        } else {
            return packageManager.getApplicationInfo(packageName, PackageManager.GET_META_DATA);
        }
    }

    public Bundle getMetaDataBundle() {
        Context context = this.getApplicationContext();
        String packageName = context.getPackageName();
        PackageManager packageManager = context.getPackageManager();

        try {
            ApplicationInfo ai = this.getPackageApplicationInfo(packageManager, packageName);

            Bundle bundle = ai.metaData;

            return bundle;
        } catch (PackageManager.NameNotFoundException e) {
            MengineLog.logError(TAG, "Unable to load meta-data: %s"
                , e.getLocalizedMessage()
            );
        }

        return null;
    }

    public boolean hasMetaData(String name) {
        Bundle bundle = this.getMetaDataBundle();

        if (bundle == null) {
            return false;
        }

        if (bundle.containsKey(name) == false) {
            return false;
        }

        return true;
    }

    public String getMetaDataString(String name) {
        Bundle bundle = this.getMetaDataBundle();

        if (bundle == null) {
            return null;
        }

        String value = bundle.getString(name);

        return value;
    }

    public boolean getMetaDataBoolean(String name, boolean d) {
        Bundle bundle = this.getMetaDataBundle();

        if (bundle == null) {
            return d;
        }

        boolean value = bundle.getBoolean(name, d);

        return value;
    }

    public int getMetaDataInteger(String name, int d) {
        Bundle bundle = this.getMetaDataBundle();

        if (bundle == null) {
            return d;
        }

        int value = bundle.getInt(name, d);

        return value;
    }

    public String getAndroidId() {
        return m_androidId;
    }

    public String getInstallKey() {
        return m_installKey;
    }

    public long getInstallKeyTimestamp() {
        return m_installTimestamp;
    }

    public String getInstallDate() {
        return m_installDate;
    }

    public long getInstallRND() {
        return m_installRND;
    }

    public long getSessionIndex() {
        return m_sessionIndex;
    }

    public void setSessionId(String sessionId) {
        if (m_sessionId.equals(sessionId) == true) {
            return;
        }

        m_sessionId = sessionId;

        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString("session_id", m_sessionId);
        editor.apply();

        this.setState("user.session_id", m_sessionId);

        this.sendEvent(MengineEvent.EVENT_SESSION_ID, m_sessionId);
    }

    public String getSessionId() {
        return m_sessionId;
    }

    public String getDeviceLanguage() {
        String language = Locale.getDefault().getLanguage();

        return language;
    }

    public String getDeviceName() {
        String deviceModel = this.getDeviceModel();

        return "Android " + deviceModel;
    }

    public String getDeviceModel() {
        String deviceName = android.os.Build.MODEL;

        return deviceName;
    }

    public int getSDKVersion() {
        int SDKVersion = Build.VERSION.SDK_INT;

        return SDKVersion;
    }

    public String getOSVersion() {
        String OSVersion = Build.VERSION.RELEASE;

        return OSVersion;
    }

    @SuppressWarnings("deprecation")
    private PackageInfo getPackageInfo(PackageManager manager, String packageName) {
        PackageInfo packageInfo;

        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                packageInfo = manager.getPackageInfo(packageName, PackageManager.PackageInfoFlags.of(0));
            } else {
                packageInfo = manager.getPackageInfo(packageName, 0);
            }
        } catch (PackageManager.NameNotFoundException e) {
            return null;
        }

        return packageInfo;
    }

    private String getSecureAndroidId() {
        Context applicationContext = this.getApplicationContext();

        ContentResolver resolver = applicationContext.getContentResolver();
        String androidId = Settings.Secure.getString(resolver, Settings.Secure.ANDROID_ID);

        return androidId;
    }

    public void setState(String name, Object value) {
        m_states.put(name, value);

        synchronized (m_syncState) {
            ArrayList<MenginePlugin> plugins = this.getPlugins();

            for (MenginePlugin p : plugins) {
                p.onState(this, name, value);
            }
        }
    }

    public Map<String, Object> getStates() {
        return m_states;
    }

    public ArrayList<MenginePlugin> getPlugins() {
        return m_plugins;
    }

    public ArrayList<MenginePluginLoggerListener> getLoggerListeners() {
        return m_loggerListeners;
    }

    public ArrayList<MenginePluginAnalyticsListener> getAnalyticsListeners() {
        return m_analyticsListeners;
    }

    public ArrayList<MenginePluginActivityLifecycleListener> getActivityLifecycleListeners() {
        return m_activityLifecycleListeners;
    }

    public ArrayList<MenginePluginKeyListener> getKeyListeners() {
        return m_keyListeners;
    }

    public ArrayList<MenginePluginApplicationListener> getApplicationListeners() {
        return m_applicationListeners;
    }

    public ArrayList<MenginePluginActivityListener> getActivityListeners() {
        return m_activityListeners;
    }

    public ArrayList<MenginePluginExtensionListener> getExtensionListeners() {
        return m_extensionListeners;
    }

    public ArrayList<MenginePluginEngineListener> getEngineListeners() {
        return m_engineListeners;
    }

    public MenginePlugin findPlugin(String name) {
        MenginePlugin plugin = m_dictionaryPlugins.get(name);

        return plugin;
    }

    @SuppressWarnings("unchecked")
    public <T> T getPlugin(Class<T> cls) {
        String name;

        try {
            Field PLUGIN_NAME = cls.getField("PLUGIN_NAME");

            name = (String)PLUGIN_NAME.get(null);
        } catch (NoSuchFieldException ex) {
            MengineLog.logError(TAG, "plugin not found field PLUGIN_NAME: %s"
                , cls.getName()
            );

            return null;
        } catch (IllegalAccessException ex) {
            MengineLog.logError(TAG, "plugin invalid field PLUGIN_NAME: %s"
                , cls.getName()
            );

            return null;
        }

        MenginePlugin plugin = this.findPlugin(name);

        return (T)plugin;
    }

    protected boolean createPlugin(String type) {
        MenginePlugin plugin = MengineUtils.newInstance(TAG, type, true);

        if (plugin == null) {
            MengineLog.logError(TAG, "invalid create instance plugin: %s"
                , type
            );

            return false;
        }

        Class<?> cls = plugin.getClass();

        String name;

        try {
            Field PLUGIN_NAME = cls.getField("PLUGIN_NAME");

            name = (String)PLUGIN_NAME.get(plugin);
        } catch (NoSuchFieldException ex) {
            MengineLog.logError(TAG, "plugin [%s] not found field PLUGIN_NAME"
                , type
            );

            return false;
        } catch (IllegalAccessException ex) {
            MengineLog.logError(TAG, "plugin [%s] invalid field PLUGIN_NAME"
                , type
            );

            return false;
        }

        if (plugin.onInitialize(this, name) == false) {
            MengineLog.logError(TAG, "plugin [%s] invalid initialize"
                , type
            );

            return false;
        }

        m_plugins.add(plugin);
        m_dictionaryPlugins.put(name, plugin);

        if (plugin instanceof MenginePluginAnalyticsListener) {
            MenginePluginAnalyticsListener listener = (MenginePluginAnalyticsListener)plugin;

            m_analyticsListeners.add(listener);
        }

        if (plugin instanceof MenginePluginLoggerListener) {
            MenginePluginLoggerListener listener = (MenginePluginLoggerListener)plugin;

            m_loggerListeners.add(listener);
        }

        if (plugin instanceof MenginePluginActivityLifecycleListener) {
            MenginePluginActivityLifecycleListener listener = (MenginePluginActivityLifecycleListener)plugin;

            m_activityLifecycleListeners.add(listener);
        }

        if (plugin instanceof MenginePluginKeyListener) {
            MenginePluginKeyListener listener = (MenginePluginKeyListener)plugin;

            m_keyListeners.add(listener);
        }

        if (plugin instanceof MenginePluginApplicationListener) {
            MenginePluginApplicationListener listener = (MenginePluginApplicationListener)plugin;

            m_applicationListeners.add(listener);
        }

        if (plugin instanceof MenginePluginActivityListener) {
            MenginePluginActivityListener listener = (MenginePluginActivityListener)plugin;

            m_activityListeners.add(listener);
        }

        if (plugin instanceof MenginePluginExtensionListener) {
            MenginePluginExtensionListener listener = (MenginePluginExtensionListener)plugin;

            m_extensionListeners.add(listener);
        }

        if (plugin instanceof MenginePluginEngineListener) {
            MenginePluginEngineListener listener = (MenginePluginEngineListener)plugin;

            m_engineListeners.add(listener);
        }

        MengineLog.logMessage(TAG, "create plugin: %s [%s]"
            , type
            , name
        );

        return true;
    }

    public MenginePluginExtension createPluginExtension(MengineActivity activity, MenginePlugin plugin, String type) {
        String pluginName = plugin.getPluginName();

        MenginePluginExtension extension = MengineUtils.newInstance(pluginName, type, false);

        if (extension == null) {
            return null;
        }

        if (extension.onPluginExtensionInitialize(this, activity, plugin) == false) {
            return null;
        }

        ArrayList<MenginePluginExtensionListener> extensionListeners = this.getExtensionListeners();

        for (MenginePluginExtensionListener l : extensionListeners) {
            l.onMenginePluginExtension(this, activity, plugin, extension);
        }

        return extension;
    }

    public void onMengineCaughtException(Throwable throwable) {
        MengineLog.logInfo(TAG, "onMengineCaughtException: %s"
            , throwable.getLocalizedMessage()
        );

        String message = throwable.getLocalizedMessage();
        this.setState("exception.message", message);

        String stacktrace = MengineUtils.getThrowableStackTrace(throwable);
        this.setState("exception.stacktrace", stacktrace);

        ArrayList<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            l.onMengineCaughtException(this, throwable);
        }
    }

    public void onMengineInitializeBaseServices(MengineActivity activity) {
        m_activityLifecycle = new MengineActivityLifecycle(this, activity);

        this.registerActivityLifecycleCallbacks(m_activityLifecycle);
    }

    public void onMengineCreateApplication(MengineActivity activity) {
        MengineLog.logInfo(TAG, "onMengineCreateApplication");

        //Empty
    }

    public void onMenginePlatformRun(MengineActivity activity) {
        MengineLog.logInfo(TAG, "onMenginePlatformRun");

        //Empty
    }

    public void onMenginePlatformReady(MengineActivity activity) {
        MengineLog.logInfo(TAG, "onMenginePlatformReady");

        //Empty
    }

    public void onMenginePlatformStop(MengineActivity activity) {
        MengineLog.logInfo(TAG, "onMenginePlatformStop");

        if (m_activityLifecycle != null) {
            this.unregisterActivityLifecycleCallbacks(m_activityLifecycle);
            m_activityLifecycle = null;
        }
    }

    public SharedPreferences getPrivateSharedPreferences(@NonNull String tag) {
        Context applicationContext = this.getApplicationContext();

        String packageName = applicationContext.getPackageName();

        SharedPreferences settings = applicationContext.getSharedPreferences(packageName + "." + tag, MODE_PRIVATE);

        return settings;
    }

    public void showToast(String format, Object ... args) {
        String message = MengineLog.buildTotalMsg(format, args);

        Toast toast = Toast.makeText(this, message, Toast.LENGTH_SHORT);

        toast.show();
    }

    public void showAlertDialog(String format, Object ... args) {
        String message = MengineLog.buildTotalMsg(format, args);

        MengineLog.logInfo(TAG, "show alert dialog: %s"
            , message
        );

        Context context = this.getApplicationContext();

        AlertDialog.Builder builder = new AlertDialog.Builder(context);

        builder.setMessage(message);
        builder.setCancelable(false);
        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });

        AlertDialog alert = builder.create();

        alert.show();
    }

    public void sendEvent(MengineEvent event, Object ... args) {
        ArrayList<MenginePlugin> plugins = this.getPlugins();

        synchronized (m_syncEvent) {
            for (MenginePlugin p : plugins) {
                p.onEvent(this, event, args);
            }
        }
    }

    @Override
    public void onCreate() {
        this.setState("build.debug", BuildConfig.DEBUG);

        this.setState("application.init", "started");

        super.onCreate();

        this.setState("application.init", "create");

        try {
            SDL.loadLibrary("SDLApplication");
        } catch (UnsatisfiedLinkError e) {
            this.invalidInitialize("load library SDLApplication catch UnsatisfiedLinkError: %s"
                , e.getLocalizedMessage()
            );
        } catch (Exception e) {
            this.invalidInitialize("load library SDLApplication catch Exception: %s"
                , e.getLocalizedMessage()
            );
        }

        this.setState("application.init", "sdl_init");

        AndroidEnv_setMengineAndroidApplicationJNI(this);

        MengineLog.setMengineApplication(this);
        MengineAnalytics.setMengineApplication(this);

        MengineLog.logInfo(TAG, "onCreate");

        if (this.getMetaDataBoolean("mengine.secure.allow_android_id", true) == true) {
            m_androidId = this.getSecureAndroidId();
        } else {
            m_androidId = "0000000000000000";
        }

        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);

        String installKey = settings.getString("install_key", null);
        long installTimestamp = settings.getLong("install_timestamp", 0);
        String installDate = settings.getString("install_date", "");
        long installRND = settings.getLong("install_rnd", -1);
        long sessionIndex = settings.getLong("session_index", 0);
        String sessionId = settings.getString("session_id", null);

        SharedPreferences.Editor editor = settings.edit();

        if (installKey == null) {
            installKey = MengineUtils.getRandomUUIDString();
            installTimestamp = MengineUtils.getTimestamp();
            installDate = MengineUtils.getDateFormat("d MMM yyyy HH:mm:ss");

            editor.putString("install_key", installKey);
            editor.putLong("install_timestamp", installTimestamp);
            editor.putString("install_date", installDate);
        }

        if (sessionId == null) {
            sessionId = installKey;

            editor.putString("session_id", sessionId);
        }

        if (installRND == -1) {
            installRND = MengineUtils.getRandomNumber();

            if (installRND == 0) {
                installRND = 1;
            } else if (installRND < 0) {
                installRND = -installRND;
            }

            editor.putLong("install_rnd", installRND);
        }

        editor.putLong("session_index", sessionIndex + 1);
        editor.apply();

        m_installKey = installKey;
        m_installTimestamp = installTimestamp;
        m_installDate = installDate;
        m_installRND = installRND;
        m_sessionIndex = sessionIndex;
        m_sessionId = sessionId;

        String gitsha1 = this.getEngineGITSHA1();
        this.setState("engine.gitsha1", gitsha1);

        String build_date = this.getBuildDate();
        this.setState("engine.build_date", build_date);

        String build_username = this.getBuildUsername();
        this.setState("engine.build_username", build_username);

        this.setState("user.install_key", m_installKey);
        this.setState("user.install_timestamp", m_installTimestamp);
        this.setState("user.install_date", m_installDate);
        this.setState("user.install_rnd", m_installRND);
        this.setState("user.session_index", m_sessionIndex);
        this.setState("user.session_id", m_sessionId);

        String sessionStartDate = MengineUtils.getDateFormat("d MMM yyyy HH:mm:ss");
        this.setState("user.session_date", sessionStartDate);

        long sessionStartTimestamp = MengineUtils.getTimestamp();
        this.setState("user.session_timestamp", sessionStartTimestamp);

        this.setState("application.init", "load");

        ArrayList<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        for (MenginePluginApplicationListener l : applicationListeners) {
            try {
                l.onAppPrepare(this);
            } catch (MenginePluginInvalidInitializeException e) {
                this.invalidInitialize("invalid plugin %s onAppCreate exception: %s"
                    , e.getPluginName()
                    , e.getLocalizedMessage()
                );
            }
        }

        this.setState("application.init", "plugins_prepare");

        if (m_sessionId.isEmpty() == false) {
            this.sendEvent(MengineEvent.EVENT_SESSION_ID, m_sessionId);
        }

        long app_init_start_timestamp = MengineAnalytics.buildEvent("mng_app_init_start")
            .log();

        for (MenginePluginApplicationListener l : applicationListeners) {
            try {
                String pluginName = l.getPluginName();

                this.setState("application.init", pluginName);

                long app_init_plugin_start_timestamp = MengineAnalytics.buildEvent("mng_app_init_plugin_start")
                    .addParameterString("name", pluginName)
                    .log();

                l.onAppCreate(this);

                MengineAnalytics.buildEvent("mng_app_init_plugin_completed")
                    .addParameterString("name", pluginName)
                    .addParameterLong("time", MengineUtils.getDurationTimestamp(app_init_plugin_start_timestamp))
                    .log();
            } catch (MenginePluginInvalidInitializeException e) {
                this.invalidInitialize("invalid plugin %s callback onAppCreate exception: %s"
                    , e.getPluginName()
                );
            }
        }

        this.setState("application.init", "completed");

        MengineAnalytics.buildEvent("mng_app_init_completed")
            .addParameterLong("time", MengineUtils.getDurationTimestamp(app_init_start_timestamp))
            .log();
    }

    @Override
    public void onTerminate() {
        MengineLog.logInfo(TAG, "onTerminate");

        ArrayList<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        for (MenginePluginApplicationListener l : applicationListeners) {
            l.onAppTerminate(this);
        }

        for (MenginePlugin plugin : m_plugins) {
            plugin.onFinalize(this);
        }

        m_plugins = null;
        m_dictionaryPlugins = null;

        m_loggerListeners = null;
        m_analyticsListeners = null;
        m_activityLifecycleListeners = null;
        m_keyListeners = null;
        m_applicationListeners = null;
        m_activityListeners = null;
        m_extensionListeners = null;
        m_engineListeners = null;

        AndroidEnv_removeMengineAndroidApplicationJNI();

        super.onTerminate();
    }

    @Override
    public void attachBaseContext(Context base) {
        super.attachBaseContext(base);

        MultiDex.install(this);

        MengineLog.logInfo(TAG, "attachBaseContext");

        ArrayList<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        for (MenginePluginApplicationListener l : applicationListeners) {
            l.onAppAttachBaseContext(this, base);
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        MengineLog.logInfo(TAG, "onConfigurationChanged config: %s"
            , newConfig.toString()
        );

        ArrayList<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        for (MenginePluginApplicationListener l : applicationListeners) {
            l.onAppConfigurationChanged(this, newConfig);
        }
    }

    public void onMengineLogger(String category, int level, int filter, int color, String msg) {
        ArrayList<MenginePluginLoggerListener> listeners = this.getLoggerListeners();

        for(MenginePluginLoggerListener l : listeners) {
            l.onMengineLogger(this, category, level, filter, color, msg);
        }
    }

    public void onMengineAnalyticsEvent(int eventType, String eventName, long timestamp, Map<String, Object> context, Map<String, Object> parameters) {
        ArrayList<MenginePluginAnalyticsListener> listeners = this.getAnalyticsListeners();

        for (MenginePluginAnalyticsListener l : listeners) {
            l.onMengineAnalyticsEvent(this, eventType, eventName, timestamp, context, parameters);
        }
    }

    private void invalidInitialize(String format, Object ... args) {
        String msg = MengineLog.logError(TAG, format, args);

        throw new RuntimeException(msg);
    }
}