package org.Mengine.Base;

import android.app.Application;
import android.content.ContentResolver;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
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

import org.libsdl.app.SDL;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.lang.reflect.Field;
import java.net.HttpURLConnection;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.net.URL;

public class MengineApplication extends Application {
    private static final String TAG = "MengineApplication";


    /*
    static {
        System.loadLibrary("SDLApplication");
    }
     */


    private static native void AndroidEnv_setMengineAndroidApplicationJNI(Object activity, ClassLoader cl);
    private static native void AndroidEnv_removeMengineAndroidApplicationJNI();

    private static native boolean AndroidEnv_isMasterRelease();
    private static native String AndroidEnv_getEngineGITSHA1();
    private static native String AndroidEnv_getBuildDate();
    private static native String AndroidEnv_getBuildUsername();

    public boolean isMasterRelease() {
        return AndroidEnv_isMasterRelease();
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
    private String m_installVersion;
    private long m_installRND = -1;
    private long m_sessionIndex = -1;
    private String m_sessionId;
    private long m_sessionTimestamp = -1;
    private long m_purchasesTimestamp = -1;

    private boolean m_invalidInitialize = false;
    private String m_invalidInitializeReason = null;

    private boolean m_networkAvailable = false;
    private boolean m_networkUnmetered = false;
    private MengineNetworkTransport m_networkTransport = MengineNetworkTransport.NETWORKTRANSPORT_UNKNOWN;
    private ConnectivityManager.NetworkCallback m_networkCallback;

    private ArrayList<MenginePlugin> m_plugins = new ArrayList<>();
    private Map<String, MenginePlugin> m_dictionaryPlugins = new HashMap<>();
    private Map<String, Object> m_states = new ConcurrentHashMap<>();

    private ArrayList<MenginePluginLoggerListener> m_loggerListeners = new ArrayList<>();
    private ArrayList<MenginePluginAnalyticsListener> m_analyticsListeners = new ArrayList<>();
    private ArrayList<MenginePluginAdRevenueListener> m_adRevenueListeners = new ArrayList<>();
    private ArrayList<MenginePluginInAppPurchaseListener> m_inAppPurchaseListeners = new ArrayList<>();
    private ArrayList<MenginePluginRemoteMessageListener> m_removeMessageListeners = new ArrayList<>();
    private ArrayList<MenginePluginKeyListener> m_keyListeners = new ArrayList<>();
    private ArrayList<MenginePluginApplicationListener> m_applicationListeners = new ArrayList<>();
    private ArrayList<MenginePluginActivityListener> m_activityListeners = new ArrayList<>();
    private ArrayList<MenginePluginEngineListener> m_engineListeners = new ArrayList<>();

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

    private Bundle assertMetaDataBundle(String name) {
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

        this.sendEvent(MengineEvent.EVENT_SESSION_ID, m_sessionId);
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
        Context context = this.getApplicationContext();

        ContentResolver resolver = context.getContentResolver();
        String androidId = Settings.Secure.getString(resolver, Settings.Secure.ANDROID_ID);

        return androidId;
    }

    public void setState(String name, Object value) {
        m_states.put(name, value);

        synchronized (m_syncState) {
            List<MenginePlugin> plugins = this.getPlugins();

            for (MenginePlugin p : plugins) {
                p.onState(this, name, value);
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
            MengineLog.logError(TAG, "[ERROR] plugin not found field PLUGIN_NAME: %s"
                , cls.getName()
            );

            return null;
        } catch (IllegalAccessException ex) {
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
        } catch (NoSuchFieldException ex) {
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

    public void sendEvent(MengineEvent event, Object ... args) {
        List<MenginePlugin> plugins = this.getPlugins();

        synchronized (m_syncEvent) {
            for (MenginePlugin p : plugins) {
                p.onEvent(this, event, args);
            }
        }
    }

    private boolean isMainProcess() {
        String packageName = this.getPackageName();

        String processName = null;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.P) {
            processName = this.getProcessName();
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

                m_networkAvailable = true;
            }

            @Override
            public void onLost(@NonNull Network network) {
                super.onLost(network);

                MengineLog.logMessage(TAG, "NetworkCallback onLost");

                m_networkAvailable = false;
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

                return MengineNetworkTransport.NETWORKTRANSPORT_UNKNOWN;
            }

            @Override
            public void onCapabilitiesChanged(@NonNull Network network, @NonNull NetworkCapabilities networkCapabilities) {
                super.onCapabilitiesChanged(network, networkCapabilities);
                final boolean unmetered = networkCapabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_NOT_METERED);

                MengineNetworkTransport transport = this.getNetworkTransport(networkCapabilities);

                if (m_networkUnmetered == unmetered && m_networkTransport == transport) {
                    return;
                }

                MengineLog.logMessage(TAG, "NetworkCallback onCapabilitiesChanged unmetered: %b transport: %s"
                    , unmetered
                    , transport
                );

                m_networkUnmetered = unmetered;
                m_networkTransport = transport;
            }
        };

        Context context = this.getApplicationContext();

        ConnectivityManager connectivityManager = (ConnectivityManager)context.getSystemService(ConnectivityManager.class);
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

        ConnectivityManager connectivityManager = (ConnectivityManager)context.getSystemService(ConnectivityManager.class);
        connectivityManager.registerDefaultNetworkCallback(m_networkCallback);

        m_networkCallback = null;
    }

    public MengineTransparencyConsentParam makeTransparencyConsentParam() {
        MengineTransparencyConsentParam consent = new MengineTransparencyConsentParam();

        Context context = this.getApplicationContext();
        consent.initFromDefaultSharedPreferences(context);

        return consent;
    }

    public boolean isInvalidInitialize() {
        return m_invalidInitialize;
    }

    public String getInvalidInitializeReason() {
        return m_invalidInitializeReason;
    }

    @Override
    public void onCreate() {
        super.onCreate();

        if (m_invalidInitialize == true) {
            return;
        }

        boolean isMainProcess = this.isMainProcess();
        String versionName = this.getVersionName();
        int versionCode = this.getVersionCode();

        MengineLog.logMessage(TAG, "onCreate isMainProcess: %b VersionName: %s [%d]"
            , isMainProcess
            , versionName
            , versionCode
        );

        List<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        for (MenginePluginApplicationListener l : applicationListeners) {
            try {
                MengineLog.logMessage(TAG, "onAppInit plugin: %s isMainProcess: %b"
                    , l.getPluginName()
                    , isMainProcess
                );

                l.onAppInit(this, isMainProcess);
            } catch (MenginePluginInvalidInitializeException e) {
                this.invalidInitialize("onAppCreate plugin: %s exception: %s"
                    , l.getPluginName()
                    , e.getMessage()
                );

                return;
            }
        }

        if (isMainProcess == false) {
            return;
        }

        MengineLog.setMengineApplication(this);
        MengineAnalytics.setMengineApplication(this);

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
            installKey = "MNIK" + MengineUtils.getSecureRandomHexString(16).toUpperCase();
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

        if (BuildConfig.DEBUG == true) {
            String installDate = MengineUtils.getDateFormat(m_installTimestamp, "d MMM yyyy HH:mm:ss");
            this.setState("user.install_date", installDate);

            String sessionDate = MengineUtils.getDateFormat(m_sessionTimestamp, "d MMM yyyy HH:mm:ss");
            this.setState("user.session_date", sessionDate);
        }

        MengineAnalytics.addContextParameterBoolean("is_dev", BuildConfig.DEBUG);
        MengineAnalytics.addContextParameterString("install_key", m_installKey);
        MengineAnalytics.addContextParameterLong("install_timestamp", m_installTimestamp);
        MengineAnalytics.addContextParameterString("install_version", m_installVersion);
        MengineAnalytics.addContextParameterLong("install_rnd", m_installRND);
        MengineAnalytics.addContextParameterLong("session_index", m_sessionIndex);
        MengineAnalytics.addContextParameterLong("session_timestamp", m_sessionTimestamp);
        MengineAnalytics.addContextParameterLong("purchases_timestamp", m_purchasesTimestamp);

        MengineAnalytics.addContextGetterParameterLong("connection", new MengineAnalyticsGetter<Long>() {
            @Override
            public Long get() {
                if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
                    return -3L;
                }

                if (m_networkAvailable == false) {
                    return -2L;
                }

                if (m_networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_CELLULAR) {
                    return 1L;
                }

                if (m_networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_WIFI) {
                    return 2L;
                }

                if (m_networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_BLUETOOTH) {
                    return 3L;
                }

                if (m_networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_ETHERNET) {
                    return 4L;
                }

                if (m_networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_VPN) {
                    return 5L;
                }

                if (m_networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_WIFI_AWARE) {
                    return 6L;
                }

                if (m_networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_LOWPAN) {
                    return 7L;
                }

                if (m_networkTransport == MengineNetworkTransport.NETWORKTRANSPORT_USB) {
                    return 8L;
                }

                return -1L;
            }
        });

        this.setState("application.init", "get_android_id");

        if (this.getMetaDataBoolean("mengine.secure.allow_android_id") == true) {
            m_androidId = this.getSecureAndroidId();
        } else {
            m_androidId = "0000000000000000";
        }

        this.setState("application.init", "sdl_load_library");

        try {
            SDL.loadLibrary("SDLApplication");
        } catch (UnsatisfiedLinkError e) {
            MengineAnalytics.buildEvent("mng_app_init_failed")
                .addParameterThrowable("exception", e)
                .logAndFlush();

            this.invalidInitialize("load library SDLApplication catch UnsatisfiedLinkError: %s"
                , e.getMessage()
            );

            return;
        } catch (Exception e) {
            MengineAnalytics.buildEvent("mng_app_init_failed")
                .addParameterException("exception", e)
                .logAndFlush();

            this.invalidInitialize("load library SDLApplication catch Exception: %s"
                , e.getMessage()
            );

            return;
        }

        this.setState("application.init", "sdl_init");

        ClassLoader cl = MengineApplication.class.getClassLoader();

        AndroidEnv_setMengineAndroidApplicationJNI(this, cl);

        String build_gitsha1 = this.getEngineGITSHA1();
        this.setState("engine.build_gitsha1", build_gitsha1);

        String build_date = this.getBuildDate();
        this.setState("engine.build_date", build_date);

        String build_username = this.getBuildUsername();
        this.setState("engine.build_username", build_username);

        for (MenginePluginApplicationListener l : applicationListeners) {
            try {
                MengineLog.logMessage(TAG, "onAppPrepare plugin: %s"
                    , l.getPluginName()
                );

                l.onAppPrepare(this);
            } catch (MenginePluginInvalidInitializeException e) {
                MengineAnalytics.buildEvent("mng_app_init_failed")
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
                MengineAnalytics.buildEvent("mng_app_init_failed")
                    .addParameterException("exception", e)
                    .logAndFlush();

                this.invalidInitialize("onAppCreate plugin: %s exception: %s"
                    , l.getPluginName()
                    , e.getMessage()
                );

                return;
            }
        }

        this.setState("application.init", "completed");
    }

    @Override
    public void onTerminate() {
        MengineLog.logMessage(TAG, "onTerminate");

        List<MenginePluginApplicationListener> applicationListeners = this.getApplicationListeners();

        for (MenginePluginApplicationListener l : applicationListeners) {
            l.onAppTerminate(this);
        }

        for (MenginePlugin plugin : m_plugins) {
            plugin.onFinalize(this);
        }

        this.finalizeMonitorConnectivityStatusAndConnectionMetering();

        MengineLog.setMengineApplication(null);
        MengineAnalytics.setMengineApplication(null);

        m_plugins = null;
        m_dictionaryPlugins = null;
        m_states = null;

        m_loggerListeners = null;
        m_analyticsListeners = null;
        m_adRevenueListeners = null;
        m_inAppPurchaseListeners = null;
        m_removeMessageListeners = null;
        m_keyListeners = null;
        m_applicationListeners = null;
        m_activityListeners = null;
        m_engineListeners = null;

        AndroidEnv_removeMengineAndroidApplicationJNI();

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
            l.onAppConfigurationChanged(this, newConfig);
        }
    }

    public void onMengineLogger(int level, int filter, String category, String msg) {
        List<MenginePluginLoggerListener> listeners = this.getLoggerListeners();

        for(MenginePluginLoggerListener l : listeners) {
            l.onMengineLogger(this, level, filter, category, msg);
        }
    }

    public void onMengineAnalyticsEvent(String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        List<MenginePluginAnalyticsListener> listeners = this.getAnalyticsListeners();

        for (MenginePluginAnalyticsListener l : listeners) {
            l.onMengineAnalyticsEvent(this, eventName, timestamp, bases, parameters);
        }
    }

    public void onMengineAnalyticsScreenView(String screenType, String screenName) {
        List<MenginePluginAnalyticsListener> listeners = this.getAnalyticsListeners();

        for (MenginePluginAnalyticsListener l : listeners) {
            l.onMengineAnalyticsScreenView(this, screenType, screenName);
        }
    }

    public void onMengineAnalyticsFlush() {
        List<MenginePluginAnalyticsListener> listeners = this.getAnalyticsListeners();

        for (MenginePluginAnalyticsListener l : listeners) {
            l.onMengineAnalyticsFlush(this);
        }
    }

    public void onMengineAdRevenue(MengineAdRevenueParam revenue) {
        List<MenginePluginAdRevenueListener> listeners = this.getAdRevenueListeners();

        for (MenginePluginAdRevenueListener l : listeners) {
            l.onMengineAdRevenue(this, revenue);
        }
    }

    public void onMengineInAppProducts(List<MengineInAppProductParam> products) {
        List<MenginePluginInAppPurchaseListener> listeners = this.getInAppAnalyticsListeners();

        for (MenginePluginInAppPurchaseListener l : listeners) {
            l.onMengineInAppProduct(this, products);
        }
    }

    public void onMengineInAppPurchase(MengineInAppPurchaseParam purchase) {
        m_purchasesTimestamp = MengineUtils.getTimestamp();

        this.setPreferenceInteger("purchases_timestamp", m_purchasesTimestamp);

        List<MenginePluginInAppPurchaseListener> listeners = this.getInAppAnalyticsListeners();

        for (MenginePluginInAppPurchaseListener l : listeners) {
            l.onMengineInAppPurchase(this, purchase);
        }
    }

    public void onMengineRemoteMessageReceived(MengineRemoteMessageParam message) {
        List<MenginePluginRemoteMessageListener> listeners = this.getRemoteMessageListeners();

        for (MenginePluginRemoteMessageListener l : listeners) {
            if (l.onMengineRemoteMessageReceived(this, message) == true) {
                break;
            }
        }
    }

    public void onMengineRemoteMessageDeleted() {
        List<MenginePluginRemoteMessageListener> listeners = this.getRemoteMessageListeners();

        for (MenginePluginRemoteMessageListener l : listeners) {
            l.onMengineRemoteMessageDeleted(this);
        }
    }

    public void onMengineRemoteMessageSent(String messageId) {
        List<MenginePluginRemoteMessageListener> listeners = this.getRemoteMessageListeners();

        for (MenginePluginRemoteMessageListener l : listeners) {
            l.onMengineRemoteMessageSent(this, messageId);
        }
    }

    public void onMengineRemoteMessageSentError(String messageId, Exception exception) {
        List<MenginePluginRemoteMessageListener> listeners = this.getRemoteMessageListeners();

        for (MenginePluginRemoteMessageListener l : listeners) {
            l.onMengineRemoteMessageSentError(this, messageId, exception);
        }
    }

    public void onMengineRemoteMessageNewToken(String token) {
        List<MenginePluginRemoteMessageListener> listeners = this.getRemoteMessageListeners();

        for (MenginePluginRemoteMessageListener l : listeners) {
            l.onMengineRemoteMessageNewToken(this, token);
        }
    }

    private void invalidInitialize(String format, Object ... args) {
        String msg = MengineLog.logError(TAG, format, args);

        m_invalidInitialize = true;
        m_invalidInitializeReason = msg;
    }
}