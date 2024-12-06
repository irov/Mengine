package org.Mengine.Base;

import android.app.Application;
import android.content.ContentResolver;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;
import android.content.res.Configuration;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;

import androidx.annotation.NonNull;
import androidx.multidex.MultiDex;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class MengineApplication extends Application {
    private static final String TAG = "MengineApplication";

    public boolean isMasterRelease() {
        return MengineNative.AndroidEnv_isMasterRelease();
    }

    public String getEngineGITSHA1() {
        return MengineNative.AndroidEnv_getEngineGITSHA1();
    }

    public String getEngineVersion() {
        return MengineNative.AndroidEnv_getEngineVersion();
    }

    public String getBuildDate() {
        return MengineNative.AndroidEnv_getBuildDate();
    }

    public String getBuildUsername() {
        return MengineNative.AndroidEnv_getBuildUsername();
    }

    private String m_androidId;

    private String m_installKey;
    private long m_installTimestamp = -1;
    private String m_installVersion;
    private long m_installRND = -1;
    private long m_sessionIndex = -1;
    private String m_sessionId;
    private long m_sessionTimestamp = -1;
    private long m_purchasesTimestamp = -1;

    private boolean m_invalidInitialize = false;
    private String m_invalidInitializeReason = null;

    private MengineActivity m_activity;

    private MengineMain m_main;

    private Object m_nativeApplication;

    private boolean m_isMengineInitializeBaseServices = false;
    private boolean m_isMengineCreateApplication = false;
    private boolean m_isMenginePlatformRun = false;

    private ConnectivityManager.NetworkCallback m_networkCallback;

    private final ArrayList<MenginePlugin> m_plugins = new ArrayList<>();
    private final Map<String, MenginePlugin> m_dictionaryPlugins = new HashMap<>();
    private final Map<String, Object> m_states = new ConcurrentHashMap<>();

    private final ArrayList<MenginePluginLoggerListener> m_loggerListeners = new ArrayList<>();
    private final ArrayList<MenginePluginAnalyticsListener> m_analyticsListeners = new ArrayList<>();
    private final ArrayList<MenginePluginAdRevenueListener> m_adRevenueListeners = new ArrayList<>();
    private final ArrayList<MenginePluginTransparencyConsentListener> m_transparencyConsentListeners = new ArrayList<>();
    private final ArrayList<MenginePluginInAppPurchaseListener> m_inAppPurchaseListeners = new ArrayList<>();
    private final ArrayList<MenginePluginRemoteMessageListener> m_removeMessageListeners = new ArrayList<>();
    private final ArrayList<MenginePluginKeyListener> m_keyListeners = new ArrayList<>();
    private final ArrayList<MenginePluginApplicationListener> m_applicationListeners = new ArrayList<>();
    private final ArrayList<MenginePluginActivityListener> m_activityListeners = new ArrayList<>();
    private final ArrayList<MenginePluginEngineListener> m_engineListeners = new ArrayList<>();
    private final ArrayList<MenginePluginPushTokenListener> m_pushTokenListeners = new ArrayList<>();
    private final ArrayList<MenginePluginAdvertisingIdListener> m_advertisingIdListeners = new ArrayList<>();
    private final ArrayList<MenginePluginSessionIdListener> m_sessionIdListeners = new ArrayList<>();

    private final Object m_syncEvent = new Object();
    private final Object m_syncState = new Object();

    public String[] getAndroidPlugins() {
        String[] plugins = {};

        return plugins;
    }

    public String[] getAndroidActivities() {
        String[] activities = {};

        return activities;
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

    public boolean isBuildPublish() {
        return false;
    }

    public String getApplicationOptions() {
        return "";
    }

    public MengineApplication() {
        super();

        String applicationId = this.getApplicationId();
        String versionName = this.getVersionName();
        int versionCode = this.getVersionCode();

        MengineLog.logMessageRelease(TAG, "MengineApplication id: %s version: %s [%d]"
            , applicationId
            , versionName
            , versionCode
        );

        String[] plugins = this.getAndroidPlugins();

        for (String namePlugin : plugins) {
            if (this.createPlugin(namePlugin) == false) {
                this.invalidInitialize("invalid create plugin: %s"
                    , namePlugin
                );

                return;
            }
        }
    }

    public Bundle getMetaDataBundle() {
        Context context = this.getApplicationContext();
        String packageName = context.getPackageName();
        PackageManager packageManager = context.getPackageManager();

        try {
            ApplicationInfo ai = MengineUtils.getPackageApplicationInfo(packageManager, packageName);

            Bundle bundle = ai.metaData;

            return bundle;
        } catch (final PackageManager.NameNotFoundException e) {
            MengineLog.logError(TAG, "[ERROR] Unable to load meta-data: %s"
                , e.getMessage()
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

    private Bundle assertMetaDataBundle(String name) throws RuntimeException{
        Bundle bundle = this.getMetaDataBundle();

        if (bundle == null) {
            String msg = String.format(Locale.US, "invalid get meta data bundle for [%s]", name);
            throw new RuntimeException(msg);
        }

        if (bundle.containsKey(name) == false) {
            String msg = String.format(Locale.US, "invalid setup meta data [%s]", name);
            throw new RuntimeException(msg);
        }

        return bundle;
    }

    public String getMetaDataString(String name) {
        Bundle bundle = this.assertMetaDataBundle(name);

        String value = bundle.getString(name);

        if (value == null) {
            String msg = String.format(Locale.US, "invalid setup meta data [%s]", name);
            throw new RuntimeException(msg);
        }

        return value;
    }

    public boolean getMetaDataBoolean(String name) {
        Bundle bundle = this.assertMetaDataBundle(name);

        boolean value = bundle.getBoolean(name);

        return value;
    }

    public int getMetaDataInteger(String name) {
        Bundle bundle = this.assertMetaDataBundle(name);

        int value = bundle.getInt(name);

        return value;
    }

    public long getMetaDataLong(String name) {
        Bundle bundle = this.assertMetaDataBundle(name);

        long value = bundle.getLong(name);

        return value;
    }

    public long getSecureRandomNumber() {
        long rnd = MengineUtils.getSecureRandomNumber();

        return rnd;
    }

    public String getSecureRandomHexString(int length) {
        String hex = MengineUtils.getSecureRandomHexString(length);

        return hex;
    }

    public boolean hasOption(String option) {
        if (BuildConfig.DEBUG == false) {
            return false;
        }

        String options_str = this.getApplicationOptions();

        String [] options = options_str.split(" ");

        if (options.length == 0) {
            return false;
        }

        String hyphen_option = "-" + option;
        String double_hyphen_option = "--" + option;

        for(String o : options) {
            if (o.equals(hyphen_option) == true) {
                return true;
            }

            if (o.equals(double_hyphen_option) == true) {
                return true;
            }
        }

        return false;
    }

    public String getAndroidId() {
        return m_androidId;
    }

    public String getInstallKey() {
        return m_installKey;
    }

    public long getInstallTimestamp() {
        return m_installTimestamp;
    }

    public String getInstallVersion() {
        return m_installVersion;
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

        this.setPreferenceString("session_id", m_sessionId);

        this.setState("user.session_id", m_sessionId);

        List<MenginePluginSessionIdListener> listeners = this.getSessionIdListeners();

        for (MenginePluginSessionIdListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineSetSessionId(this, m_sessionId);
        }
    }

    public void removeSessionData() {
        this.clearPreferences();

        List<MenginePluginSessionIdListener> listeners = this.getSessionIdListeners();

        for (MenginePluginSessionIdListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineRemoveSessionData(this);
        }
    }

    public String getSessionId() {
        return m_sessionId;
    }

    public long getSessionTimestamp() {
        return m_sessionTimestamp;
    }

    public long getPurchasesTimestamp() {
        return m_purchasesTimestamp;
    }

    public String getDeviceModel() {
        String deviceName = android.os.Build.MODEL;

        return deviceName;
    }

    public String getDeviceLanguage() {
        Locale locale = Locale.getDefault();
        String language = locale.getLanguage();

        return language;
    }

    public int getSDKVersion() {
        int SDKVersion = Build.VERSION.SDK_INT;

        return SDKVersion;
    }

    public String getOSVersion() {
        String OSVersion = Build.VERSION.RELEASE;

        return OSVersion;
    }

    private String getSecureAndroidId() {
        Context context = this.getApplicationContext();

        ContentResolver resolver = context.getContentResolver();
        String androidId = Settings.Secure.getString(resolver, Settings.Secure.ANDROID_ID);

        if (androidId == null) {
            return "0000000000000000";
        }

        return androidId;
    }

    public void setMengineActivity(MengineActivity activity) {
        m_activity = activity;
    }

    public MengineActivity getMengineActivity() {
        return m_activity;
    }

    public boolean isMengineInitializeBaseServices() {
        return m_isMengineInitializeBaseServices;
    }

    public boolean isMengineCreateApplication() {
        return m_isMengineCreateApplication;
    }

    public boolean isMenginePlatformRun() {
        return m_isMenginePlatformRun;
    }

    public void setState(String name, Object value) {
        m_states.put(name, value);

        List<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        synchronized (m_syncState) {
            for (MenginePluginApplicationListener listener : applicationListeners) {
                if (listener.onAvailable(this) == false) {
                    continue;
                }

                listener.onAppState(this, name, value);
            }
        }
    }

    public Map<String, Object> getStates() {
        return m_states;
    }

    public List<MenginePlugin> getPlugins() {
        return m_plugins;
    }

    public List<MenginePluginLoggerListener> getLoggerListeners() {
        return m_loggerListeners;
    }

    public List<MenginePluginAnalyticsListener> getAnalyticsListeners() {
        return m_analyticsListeners;
    }

    public List<MenginePluginAdRevenueListener> getAdRevenueListeners() {
        return m_adRevenueListeners;
    }

    public List<MenginePluginTransparencyConsentListener> getTransparencyConsentListeners() {
        return m_transparencyConsentListeners;
    }

    public List<MenginePluginInAppPurchaseListener> getInAppAnalyticsListeners() {
        return m_inAppPurchaseListeners;
    }

    public List<MenginePluginRemoteMessageListener> getRemoteMessageListeners() {
        return m_removeMessageListeners;
    }

    public List<MenginePluginKeyListener> getKeyListeners() {
        return m_keyListeners;
    }

    public List<MenginePluginApplicationListener> getApplicationListeners() {
        return m_applicationListeners;
    }

    public List<MenginePluginActivityListener> getActivityListeners() {
        return m_activityListeners;
    }

    public List<MenginePluginEngineListener> getEngineListeners() {
        return m_engineListeners;
    }

    public List<MenginePluginPushTokenListener> getPushTokenListeners() {
        return m_pushTokenListeners;
    }

    public List<MenginePluginAdvertisingIdListener> getAdvertisingIdListeners() {
        return m_advertisingIdListeners;
    }

    public List<MenginePluginSessionIdListener> getSessionIdListeners() {
        return m_sessionIdListeners;
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
        } catch (final NoSuchFieldException ex) {
            MengineLog.logError(TAG, "[ERROR] plugin not found field PLUGIN_NAME: %s"
                , cls.getName()
            );

            return null;
        } catch (final IllegalAccessException ex) {
            MengineLog.logError(TAG, "[ERROR] plugin invalid field PLUGIN_NAME: %s"
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
        } catch (final NoSuchFieldException ex) {
            MengineLog.logError(TAG, "[ERROR] plugin [%s] not found field PLUGIN_NAME"
                , type
            );

            return false;
        } catch (IllegalAccessException ex) {
            MengineLog.logError(TAG, "[ERROR] plugin [%s] invalid field PLUGIN_NAME"
                , type
            );

            return false;
        }

        if (name == null) {
            MengineLog.logError(TAG, "[ERROR] plugin [%s] invalid name"
                , type
            );

            return false;
        }

        if (name.length() > 23) {
            MengineLog.logError(TAG, "[ERROR] plugin [%s] invalid name length > 23"
                , type
            );

            return false;
        }

        if (plugin.onInitialize(this, name) == false) {
            MengineLog.logError(TAG, "[ERROR] plugin [%s] invalid initialize"
                , type
            );

            return false;
        }

        m_plugins.add(plugin);
        m_dictionaryPlugins.put(name, plugin);

        if (plugin instanceof MenginePluginAnalyticsListener listener) {
            m_analyticsListeners.add(listener);
        }

        if (plugin instanceof MenginePluginAdRevenueListener listener) {
            m_adRevenueListeners.add(listener);
        }

        if (plugin instanceof MenginePluginTransparencyConsentListener listener) {
            m_transparencyConsentListeners.add(listener);
        }

        if (plugin instanceof MenginePluginInAppPurchaseListener listener) {
            m_inAppPurchaseListeners.add(listener);
        }

        if (plugin instanceof MenginePluginRemoteMessageListener listener) {
            m_removeMessageListeners.add(listener);
        }

        if (plugin instanceof MenginePluginLoggerListener listener) {
            m_loggerListeners.add(listener);
        }

        if (plugin instanceof MenginePluginKeyListener listener) {
            m_keyListeners.add(listener);
        }

        if (plugin instanceof MenginePluginApplicationListener listener) {
            m_applicationListeners.add(listener);
        }

        if (plugin instanceof MenginePluginActivityListener listener) {
            m_activityListeners.add(listener);
        }

        if (plugin instanceof MenginePluginEngineListener listener) {
            m_engineListeners.add(listener);
        }

        if (plugin instanceof MenginePluginPushTokenListener listener) {
            m_pushTokenListeners.add(listener);
        }

        if (plugin instanceof MenginePluginAdvertisingIdListener listener) {
            m_advertisingIdListeners.add(listener);
        }

        if (plugin instanceof MenginePluginSessionIdListener listener) {
            m_sessionIdListeners.add(listener);
        }

        MengineLog.logMessage(TAG, "create plugin: %s [%s]"
            , type
            , name
        );

        return true;
    }

    public void onMengineCaughtException(Throwable throwable) {
        String message = throwable.getMessage();
        this.setState("exception.message", message);

        String stacktrace = MengineUtils.getThrowableStackTrace(throwable);
        this.setState("exception.stacktrace", stacktrace);

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineCaughtException(this, throwable);
        }
    }

    public SharedPreferences getPrivateSharedPreferences(@NonNull String tag) {
        Context context = this.getApplicationContext();

        String packageName = context.getPackageName();

        SharedPreferences settings = context.getSharedPreferences(packageName + "." + tag, MODE_PRIVATE);

        return settings;
    }

    public boolean hasPreference(@NonNull String name) {
        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);

        boolean has = settings.contains(name);

        return has;
    }

    public boolean getPreferenceBoolean(@NonNull String name, boolean defaultValue) {
        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);

        boolean value = settings.getBoolean(name, defaultValue);

        return value;
    }

    public void setPreferenceBoolean(@NonNull String name, boolean value) {
        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);

        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean(name, value);
        editor.apply();
    }

    public long getPreferenceInteger(@NonNull String name, long defaultValue) {
        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);

        long value = settings.getLong(name, defaultValue);

        return value;
    }

    public void setPreferenceInteger(@NonNull String name, long value) {
        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);

        SharedPreferences.Editor editor = settings.edit();
        editor.putLong(name, value);
        editor.apply();
    }

    public String getPreferenceString(@NonNull String name, String defaultValue) {
        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);

        String value = settings.getString(name, defaultValue);

        return value;
    }

    public void setPreferenceString(@NonNull String name, String value) {
        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);

        SharedPreferences.Editor editor = settings.edit();
        editor.putString(name, value);
        editor.apply();
    }

    public void removePreference(@NonNull String name) {
        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);

        SharedPreferences.Editor editor = settings.edit();
        editor.remove(name);
        editor.apply();
    }

    public void clearPreferences() {
        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);

        SharedPreferences.Editor editor = settings.edit();
        editor.clear();
        editor.apply();
    }

    public void sendEvent(MengineEvent event, Object ... args) {
        List<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        synchronized (m_syncEvent) {
            for (MenginePluginApplicationListener listener : applicationListeners) {
                if (listener.onAvailable(this) == false) {
                    continue;
                }

                listener.onAppEvent(this, event, args);
            }
        }
    }

    private boolean isMainProcess() {
        String packageName = this.getPackageName();

        String processName;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.P) {
            processName = Application.getProcessName();
        } else {
            Context context = this.getApplicationContext();
            processName = MengineUtils.getProcessNameBeforeVersionP(context);
        }

        if (processName == null) {
            return false;
        }

        if (packageName.equals(processName) == false) {
            return false;
        }

        return true;
    }

    private void initializeMonitorConnectivityStatusAndConnectionMetering() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
            return;
        }

        ConnectivityManager.NetworkCallback networkCallback = new ConnectivityManager.NetworkCallback() {
            @Override
            public void onAvailable(@NonNull Network network) {
                super.onAvailable(network);

                MengineLog.logMessage(TAG, "NetworkCallback onAvailable");

                MengineNetwork.setNetworkAvailable(true);
            }

            @Override
            public void onLost(@NonNull Network network) {
                super.onLost(network);

                MengineLog.logMessage(TAG, "NetworkCallback onLost");

                MengineNetwork.setNetworkAvailable(false);
            }

            private MengineNetworkTransport getNetworkTransport(@NonNull NetworkCapabilities networkCapabilities) {
                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_CELLULAR;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_WIFI;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_BLUETOOTH) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_BLUETOOTH;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_ETHERNET) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_ETHERNET;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_VPN) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_VPN;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI_AWARE) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_WIFI_AWARE;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_LOWPAN) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_LOWPAN;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_USB) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_USB;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_THREAD) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_THREAD;
                }

                return MengineNetworkTransport.NETWORKTRANSPORT_UNKNOWN;
            }

            @Override
            public void onCapabilitiesChanged(@NonNull Network network, @NonNull NetworkCapabilities networkCapabilities) {
                super.onCapabilitiesChanged(network, networkCapabilities);
                final boolean unmetered = networkCapabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_NOT_METERED);

                MengineNetworkTransport transport = this.getNetworkTransport(networkCapabilities);

                boolean last_unmetered = MengineNetwork.isNetworkUnmetered();
                MengineNetworkTransport last_transport = MengineNetwork.getNetworkTransport();

                if (last_unmetered == unmetered && last_transport == transport) {
                    return;
                }

                MengineLog.logMessage(TAG, "NetworkCallback onCapabilitiesChanged unmetered: %b transport: %s"
                    , unmetered
                    , transport
                );

                MengineNetwork.setNetworkUnmetered(unmetered);
                MengineNetwork.setNetworkTransport(transport);
            }
        };

        Context context = this.getApplicationContext();

        ConnectivityManager connectivityManager = context.getSystemService(ConnectivityManager.class);
        connectivityManager.registerDefaultNetworkCallback(networkCallback);

        m_networkCallback = networkCallback;
    }

    private void finalizeMonitorConnectivityStatusAndConnectionMetering() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
            return;
        }

        if (m_networkCallback == null) {
            return;
        }

        Context context = this.getApplicationContext();

        ConnectivityManager connectivityManager = context.getSystemService(ConnectivityManager.class);
        connectivityManager.unregisterNetworkCallback(m_networkCallback);

        m_networkCallback = null;
    }

    public MengineTransparencyConsentParam makeTransparencyConsentParam() {
        MengineTransparencyConsentParam tcParam = new MengineTransparencyConsentParam();

        Context context = this.getApplicationContext();
        tcParam.initFromDefaultSharedPreferences(context);

        return tcParam;
    }

    public boolean isInvalidInitialize() {
        return m_invalidInitialize;
    }

    public String getInvalidInitializeReason() {
        return m_invalidInitializeReason;
    }

    protected static String generateInstallKey() {
        String installKey = "MNIK" + MengineUtils.getSecureRandomHexString(16).toUpperCase();

        return installKey;
    }

    public Object getNativeApplication() {
        return m_nativeApplication;
    }

    @Override
    public void onCreate() {
        super.onCreate();

        if (m_invalidInitialize == true) {
            return;
        }

        MengineLog.setMengineApplication(this);
        MengineAnalytics.setMengineApplication(this);

        boolean isMainProcess = this.isMainProcess();

        MengineLog.logMessageRelease(TAG, "onCreate isMainProcess: %b "
            , isMainProcess
        );

        List<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        for (MenginePluginApplicationListener l : applicationListeners) {
            try {
                if (l.onAvailable(this) == false) {
                    continue;
                }

                MengineLog.logMessage(TAG, "onAppInit plugin: %s isMainProcess: %b"
                    , l.getPluginName()
                    , isMainProcess
                );

                l.onAppInit(this, isMainProcess);
            } catch (final MenginePluginInvalidInitializeException e) {
                this.invalidInitialize("onAppInit plugin: %s exception: %s"
                    , l.getPluginName()
                    , e.getMessage()
                );

                return;
            }
        }

        if (isMainProcess == false) {
            return;
        }

        this.setState("build.debug", BuildConfig.DEBUG);

        this.setState("application.init", "started");

        this.initializeMonitorConnectivityStatusAndConnectionMetering();

        this.setState("application.init", "load_preferences");

        SharedPreferences settings = this.getPrivateSharedPreferences(TAG);

        long MENGINE_APPLICATION_SAVE_VERSION = 1L;

        long saveVersion = settings.getLong("save_version", MENGINE_APPLICATION_SAVE_VERSION);
        String installKey = settings.getString("install_key", null);
        long installTimestamp = settings.getLong("install_timestamp", -1);
        String installVersion = settings.getString("install_version", "");
        long installRND = settings.getLong("install_rnd", -1);
        long sessionIndex = settings.getLong("session_index", 0);
        String sessionId = settings.getString("session_id", null);
        long purchasesTimestamp = settings.getLong("purchases_timestamp", 0);

        SharedPreferences.Editor editor = settings.edit();

        editor.putLong("save_version", MENGINE_APPLICATION_SAVE_VERSION);

        if (installKey == null) {
            installKey = MengineApplication.generateInstallKey();
            installTimestamp = MengineUtils.getTimestamp();
            installVersion = this.getVersionName();

            editor.putString("install_key", installKey);
            editor.putLong("install_timestamp", installTimestamp);
            editor.putString("install_version", installVersion);

            installRND = MengineUtils.getSecureRandomNumber();

            if (installRND == 0) {
                installRND = 1;
            } else if (installRND < 0) {
                installRND = -installRND;
            }

            editor.putLong("install_rnd", installRND);
        }

        if (sessionId == null) {
            sessionId = installKey;

            editor.putString("session_id", sessionId);
        }

        editor.putLong("session_index", sessionIndex + 1);
        editor.apply();

        m_installKey = installKey;
        m_installTimestamp = installTimestamp;
        m_installVersion = installVersion;
        m_installRND = installRND;
        m_sessionIndex = sessionIndex;
        m_sessionId = sessionId;
        m_sessionTimestamp = MengineUtils.getTimestamp();
        m_purchasesTimestamp = purchasesTimestamp;

        this.setState("user.install_key", m_installKey);
        this.setState("user.install_timestamp", m_installTimestamp);
        this.setState("user.install_version", m_installVersion);
        this.setState("user.install_rnd", m_installRND);
        this.setState("user.session_index", m_sessionIndex);
        this.setState("user.session_id", m_sessionId);
        this.setState("user.session_timestamp", m_sessionTimestamp);
        this.setState("user.purchases_timestamp", m_purchasesTimestamp);

        MengineAnalytics.addContextParameterBoolean("is_dev", BuildConfig.DEBUG);
        MengineAnalytics.addContextParameterString("install_key", m_installKey);
        MengineAnalytics.addContextParameterLong("install_timestamp", m_installTimestamp);
        MengineAnalytics.addContextParameterString("install_version", m_installVersion);
        MengineAnalytics.addContextParameterLong("install_rnd", m_installRND);
        MengineAnalytics.addContextParameterLong("session_index", m_sessionIndex);
        MengineAnalytics.addContextParameterLong("session_timestamp", m_sessionTimestamp);
        MengineAnalytics.addContextParameterLong("purchases_timestamp", m_purchasesTimestamp);

        MengineAnalytics.addContextGetterParameterLong("connection", () -> {
            if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
                return -3L;
            }

            boolean networkAvailable = MengineNetwork.isNetworkAvailable();

            if (networkAvailable == false) {
                return -2L;
            }

            MengineNetworkTransport networkTransport = MengineNetwork.getNetworkTransport();

            if (networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_CELLULAR) {
                return 1L;
            }

            if (networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_WIFI) {
                return 2L;
            }

            if (networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_BLUETOOTH) {
                return 3L;
            }

            if (networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_ETHERNET) {
                return 4L;
            }

            if (networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_VPN) {
                return 5L;
            }

            if (networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_WIFI_AWARE) {
                return 6L;
            }

            if (networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_LOWPAN) {
                return 7L;
            }

            if (networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_USB) {
                return 8L;
            }

            return -1L;
        });

        this.setState("application.init", "get_android_id");

        if (this.getMetaDataBoolean("mengine.secure.allow_android_id") == true) {
            m_androidId = this.getSecureAndroidId();
        } else {
            m_androidId = "0000000000000000";
        }

        try {
            Context context = this.getApplicationContext();

            MengineUtils.loadLibrary(context, "AndroidApplication");
        } catch(final UnsatisfiedLinkError e) {
            this.invalidInitialize("loadLibrary AndroidApplication UnsatisfiedLinkError: %s"
                , e.getMessage()
            );

            return;
        } catch(final SecurityException e) {
            this.invalidInitialize("loadLibrary AndroidApplication SecurityException: %s"
                , e.getMessage()
            );

            return;
        }

        ClassLoader cl = MengineApplication.class.getClassLoader();

        MengineNative.AndroidEnv_setMengineAndroidApplicationJNI(this, cl);

        String engine_gitsha1 = this.getEngineGITSHA1();
        this.setState("engine.gitsha1", engine_gitsha1);

        String engine_version = this.getEngineVersion();
        this.setState("engine.version", engine_version);

        String engine_builddate = this.getBuildDate();
        this.setState("engine.build_date", engine_builddate);

        this.setState("application.init", "plugins_prepare");

        for (MenginePluginApplicationListener l : applicationListeners) {
            try {
                MengineLog.logMessage(TAG, "onAppPrepare plugin: %s"
                    , l.getPluginName()
                );

                l.onAppPrepare(this);
            } catch (final MenginePluginInvalidInitializeException e) {
                MengineAnalytics.buildEvent("mng_app_create_failed")
                    .addParameterException("exception", e)
                    .logAndFlush();

                this.invalidInitialize("onAppPrepare plugin: %s exception: %s"
                    , l.getPluginName()
                    , e.getMessage()
                );

                return;
            }
        }

        this.setState("application.init", "plugins_prepared");

        for (MenginePluginApplicationListener l : applicationListeners) {
            try {
                String pluginName = l.getPluginName();

                MengineLog.logMessage(TAG, "onAppCreate plugin: %s"
                    , pluginName
                );

                this.setState("application.init", pluginName);

                l.onAppCreate(this);
            } catch (MenginePluginInvalidInitializeException e) {
                MengineAnalytics.buildEvent("mng_app_create_failed")
                    .addParameterException("exception", e)
                    .logAndFlush();

                this.invalidInitialize("onAppCreate plugin: %s exception: %s"
                    , l.getPluginName()
                    , e.getMessage()
                );

                return;
            }
        }

        ApplicationInfo applicationInfo = this.getApplicationInfo();
        String nativeLibraryDir = applicationInfo.nativeLibraryDir;
        String options = this.getApplicationOptions();

        String[] optionsArgs = options.split(" ");

        Object nativeApplication = MengineNative.AndroidMain_bootstrap(nativeLibraryDir, optionsArgs);

        if (nativeApplication == null) {
            MengineAnalytics.buildEvent("mng_app_create_failed")
                .addParameterString("reason", "bootstrap failed")
                .logAndFlush();

            this.invalidInitialize("bootstrap failed");

            return;
        }

        m_nativeApplication = nativeApplication;

        for (MenginePlugin p : m_plugins) {
            if (p.onAvailable(this) == false) {
                continue;
            }

            if (p.isEmbedding() == false) {
                continue;
            }

            String name = p.getPluginName();

            MengineNative.AndroidNativePython_addPlugin(name, p);
        }

        this.setState("application.init", "run_main");

        MengineNative.AndroidNativePython_addPlugin("Application", this);

        m_main = new MengineMain(m_nativeApplication);
        m_main.start();

        this.setState("application.init", "completed");
    }

    @Override
    public void onTerminate() {
        MengineLog.logMessage(TAG, "onTerminate");

        List<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        for (MenginePluginApplicationListener l : applicationListeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onAppTerminate(this);
        }

        for (MenginePlugin p : m_plugins) {
            if (p.onAvailable(this) == false) {
                continue;
            }

            if (p.isEmbedding() == false) {
                continue;
            }

            String name = p.getPluginName();

            MengineNative.AndroidNativePython_removePlugin(name);
        }

        for (MenginePlugin plugin : m_plugins) {
            plugin.onFinalize(this);
        }

        this.finalizeMonitorConnectivityStatusAndConnectionMetering();

        MengineLog.setMengineApplication(null);
        MengineAnalytics.setMengineApplication(null);

        MengineNative.AndroidNativePython_removePlugin("Application");

        m_main.stop();
        m_main = null;

        MengineNative.AndroidMain_destroy(m_nativeApplication);
        m_nativeApplication = null;

        MengineNative.AndroidEnv_removeMengineAndroidApplicationJNI();

        super.onTerminate();
    }

    @Override
    public void attachBaseContext(Context base) {
        super.attachBaseContext(base);

        MultiDex.install(this);

        MengineLog.logMessage(TAG, "attachBaseContext");

        List<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        for (MenginePluginApplicationListener l : applicationListeners) {
            l.onAppAttachBaseContext(this, base);
        }
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        MengineLog.logMessage(TAG, "onConfigurationChanged config: %s"
            , newConfig.toString()
        );

        List<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        for (MenginePluginApplicationListener l : applicationListeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onAppConfigurationChanged(this, newConfig);
        }
    }

    public void onMengineInitializeBaseServices() {
        MengineLog.logInfo(TAG, "onMengineInitializeBaseServices");

        m_isMengineInitializeBaseServices = true;

        MengineLog.initializeBaseServices();

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineInitializeBaseServices(this);
        }
    }

    public void onMengineCreateApplication() {
        MengineLog.logInfo(TAG, "onMengineCreateApplication");

        m_isMengineCreateApplication = true;

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineCreateApplication(this);
        }
    }

    public void onMenginePlatformRun() {
        MengineLog.logInfo(TAG, "onMenginePlatformRun");

        m_isMenginePlatformRun = true;

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMenginePlatformRun(this);
        }
    }

    public void onMenginePlatformStop() {
        MengineLog.logInfo(TAG, "onMenginePlatformStop");

        m_isMenginePlatformRun = false;

        List<MenginePluginEngineListener> listeners = this.getEngineListeners();

        for (MenginePluginEngineListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMenginePlatformStop(this);
        }

        MengineLog.finalizeBaseServices();
    }

    public void onMengineCurrentSceneChange(String name) {
        MengineLog.logInfo(TAG, "onMengineCurrentSceneChange: %s"
            , name
        );

        this.setState("current.scene", name);
    }

    public void onMengineLogger(int level, int filter, String category, String msg) {
        List<MenginePluginLoggerListener> listeners = this.getLoggerListeners();

        for(MenginePluginLoggerListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineLogger(this, level, filter, category, msg);
        }
    }

    public void onMengineAnalyticsEvent(String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        List<MenginePluginAnalyticsListener> listeners = this.getAnalyticsListeners();

        for (MenginePluginAnalyticsListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineAnalyticsEvent(this, eventName, timestamp, bases, parameters);
        }
    }

    public void onMengineAnalyticsScreenView(String screenType, String screenName) {
        List<MenginePluginAnalyticsListener> listeners = this.getAnalyticsListeners();

        for (MenginePluginAnalyticsListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineAnalyticsScreenView(this, screenType, screenName);
        }
    }

    public void onMengineAnalyticsFlush() {
        List<MenginePluginAnalyticsListener> listeners = this.getAnalyticsListeners();

        for (MenginePluginAnalyticsListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineAnalyticsFlush(this);
        }
    }

    public void onMengineAdRevenue(MengineAdRevenueParam revenue) {
        List<MenginePluginAdRevenueListener> listeners = this.getAdRevenueListeners();

        for (MenginePluginAdRevenueListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineAdRevenue(this, revenue);
        }
    }

    public void onMengineTransparencyConsent(MengineTransparencyConsentParam tcParam) {
        List<MenginePluginTransparencyConsentListener> listeners = this.getTransparencyConsentListeners();

        for (MenginePluginTransparencyConsentListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineTransparencyConsent(this, tcParam);
        }
    }

    public void onMengineInAppProducts(List<MengineInAppProductParam> products) {
        List<MenginePluginInAppPurchaseListener> listeners = this.getInAppAnalyticsListeners();

        for (MenginePluginInAppPurchaseListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineInAppProduct(this, products);
        }
    }

    public void onMengineInAppPurchase(MengineInAppPurchaseParam purchase) {
        m_purchasesTimestamp = MengineUtils.getTimestamp();

        this.setPreferenceInteger("purchases_timestamp", m_purchasesTimestamp);

        List<MenginePluginInAppPurchaseListener> listeners = this.getInAppAnalyticsListeners();

        for (MenginePluginInAppPurchaseListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineInAppPurchase(this, purchase);
        }
    }

    public void onMengineRemoteMessageReceived(MengineRemoteMessageParam message) {
        List<MenginePluginRemoteMessageListener> listeners = this.getRemoteMessageListeners();

        for (MenginePluginRemoteMessageListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            if (l.onMengineRemoteMessageReceived(this, message) == true) {
                break;
            }
        }
    }

    public void onMengineRemoteMessageDeleted() {
        List<MenginePluginRemoteMessageListener> listeners = this.getRemoteMessageListeners();

        for (MenginePluginRemoteMessageListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineRemoteMessageDeleted(this);
        }
    }

    public void onMengineRemoteMessageSent(String messageId) {
        List<MenginePluginRemoteMessageListener> listeners = this.getRemoteMessageListeners();

        for (MenginePluginRemoteMessageListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineRemoteMessageSent(this, messageId);
        }
    }

    public void onMengineRemoteMessageSentError(String messageId, Exception exception) {
        List<MenginePluginRemoteMessageListener> listeners = this.getRemoteMessageListeners();

        for (MenginePluginRemoteMessageListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineRemoteMessageSentError(this, messageId, exception);
        }
    }

    public void onMengineRemoteMessageNewToken(String token) {
        List<MenginePluginRemoteMessageListener> listeners = this.getRemoteMessageListeners();

        for (MenginePluginRemoteMessageListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineRemoteMessageNewToken(this, token);
        }
    }

    public void onMenginePushToken(String token) {
        List<MenginePluginPushTokenListener> listeners = this.getPushTokenListeners();

        for (MenginePluginPushTokenListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMenginePushToken(this, token);
        }
    }

    public void onMengineAdvertisingId(String advertisingId, boolean limitAdTracking) {
        this.setState("user.limit_ad_tracking", limitAdTracking);

        List<MenginePluginAdvertisingIdListener> listeners = this.getAdvertisingIdListeners();

        for (MenginePluginAdvertisingIdListener l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineAdvertisingId(this, advertisingId, limitAdTracking);
        }
    }

    private void invalidInitialize(String format, Object ... args) {
        String msg = MengineLog.logErrorException(TAG, format, args);

        m_invalidInitialize = true;
        m_invalidInitializeReason = msg;
    }
}