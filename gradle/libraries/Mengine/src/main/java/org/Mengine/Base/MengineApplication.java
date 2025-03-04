package org.Mengine.Base;

import android.app.Application;
import android.content.ContentResolver;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import com.google.common.base.Splitter;

import org.json.JSONObject;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class MengineApplication extends Application {
    private static final String TAG = "MengineApplication";

    static {
        System.loadLibrary("AndroidApplication");
    }

    private static MengineApplication m_sharedInstance;

    private String m_androidId;

    private long m_saveVersion = -1;
    private String m_installKey;
    private long m_installTimestamp = -1;
    private String m_installVersion;
    private long m_installRND = -1;
    private long m_sessionIndex = -1;
    private String m_sessionId;
    private long m_sessionTimestamp = -1;

    private boolean m_invalidInitialize = false;
    private String m_invalidInitializeReason = null;

    private MengineActivity m_activity;

    private MengineMain m_main;

    private Object m_nativeApplication;

    private boolean m_isMengineInitializeBaseServices = false;
    private boolean m_isMengineCreateApplication = false;
    private boolean m_isMenginePlatformRun = false;

    private final List<MengineService> m_services = new ArrayList<>();
    private final Map<String, MengineService> m_dictionaryServices = new HashMap<>();
    private final Map<String, Object> m_states = new ConcurrentHashMap<>();

    private final List<MengineListenerLogger> m_loggerListeners = new ArrayList<>();
    private final List<MengineListenerAnalytics> m_analyticsListeners = new ArrayList<>();
    private final List<MengineListenerAdRevenue> m_adRevenueListeners = new ArrayList<>();
    private final List<MengineListenerTransparencyConsent> m_transparencyConsentListeners = new ArrayList<>();
    private final List<MengineListenerInAppPurchase> m_inAppPurchaseListeners = new ArrayList<>();
    private final List<MengineListenerRemoteMessage> m_removeMessageListeners = new ArrayList<>();
    private final List<MengineListenerKeyEvent> m_keyListeners = new ArrayList<>();
    private final List<MengineListenerApplication> m_applicationListeners = new ArrayList<>();
    private final List<MengineListenerActivity> m_activityListeners = new ArrayList<>();
    private final List<MengineListenerEngine> m_engineListeners = new ArrayList<>();
    private final List<MengineListenerPushToken> m_pushTokenListeners = new ArrayList<>();
    private final List<MengineListenerAdvertisingId> m_advertisingIdListeners = new ArrayList<>();
    private final List<MengineListenerSessionId> m_sessionIdListeners = new ArrayList<>();
    private final List<MengineListenerPerformance> m_performanceListeners = new ArrayList<>();
    private final List<MengineListenerRemoteConfig> m_remoteConfigListeners = new ArrayList<>();

    private final Object m_syncEvent = new Object();
    private final Object m_syncState = new Object();

    public static MengineApplication getSharedInstance() {
        return m_sharedInstance;
    }

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

        m_sharedInstance = this;

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
            if (this.createService(namePlugin) == false) {
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

            MengineUtils.throwRuntimeException(msg, null);
        }

        if (bundle.containsKey(name) == false) {
            String msg = String.format(Locale.US, "invalid setup meta data [%s]", name);

            MengineUtils.throwRuntimeException(msg, null);
        }

        return bundle;
    }

    public String getMetaDataString(String name) {
        Bundle bundle = this.assertMetaDataBundle(name);

        String value = bundle.getString(name);

        if (value == null) {
            String msg = String.format(Locale.US, "invalid setup meta data [%s]", name);

            MengineUtils.throwRuntimeException(msg, null);
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

    public boolean hasOption(String option) {
        if (BuildConfig.DEBUG == false) {
            return false;
        }

        String options_str = this.getApplicationOptions();

        List<String> options = Splitter.on(' ').splitToList(options_str);

        if (options.isEmpty() == true) {
            return false;
        }

        String hyphen_option = "-" + option;
        String double_hyphen_option = "--" + option;

        String hyphen_option_value = hyphen_option + ":";
        String double_hyphen_option_value = double_hyphen_option + ":";

        for(String o : options) {
            if (o.equals(hyphen_option) == true) {
                return true;
            }

            if (o.equals(double_hyphen_option) == true) {
                return true;
            }

            if (o.startsWith(hyphen_option_value) == true) {
                return true;
            }

            if (o.startsWith(double_hyphen_option_value) == true) {
                return true;
            }
        }

        return false;
    }

    public int getOptionValueInteger(String option, int defaultValue) {
        if (BuildConfig.DEBUG == false) {
            return defaultValue;
        }

        String options_str = this.getApplicationOptions();

        List<String> options = Splitter.on(' ').splitToList(options_str);

        if (options.isEmpty() == true) {
            return defaultValue;
        }

        String hyphen_option = "-" + option;
        String double_hyphen_option = "--" + option;

        String hyphen_option_value = hyphen_option + ":";
        String double_hyphen_option_value = double_hyphen_option + ":";

        for(String o : options) {
            if (o.equals(hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.equals(double_hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.startsWith(hyphen_option_value) == true) {
                String value = o.substring(hyphen_option_value.length());

                try {
                    return Integer.parseInt(value);
                } catch (final NumberFormatException e) {
                    MengineLog.logSingleWarning(TAG, "option [%s] invalid integer value [%s]", option, value);

                    return defaultValue;
                }
            }

            if (o.startsWith(double_hyphen_option_value) == true) {
                String value = o.substring(double_hyphen_option_value.length());

                try {
                    return Integer.parseInt(value);
                } catch (final NumberFormatException e) {
                    MengineLog.logSingleWarning(TAG, "option [%s] invalid integer value [%s]", option, value);

                    return defaultValue;
                }
            }
        }

        return defaultValue;
    }

    public long getOptionValueLong(String option, long defaultValue) {
        if (BuildConfig.DEBUG == false) {
            return defaultValue;
        }

        String options_str = this.getApplicationOptions();

        List<String> options = Splitter.on(' ').splitToList(options_str);

        if (options.isEmpty() == true) {
            return defaultValue;
        }

        String hyphen_option = "-" + option;
        String double_hyphen_option = "--" + option;

        String hyphen_option_value = hyphen_option + ":";
        String double_hyphen_option_value = double_hyphen_option + ":";

        for(String o : options) {
            if (o.equals(hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.equals(double_hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.startsWith(hyphen_option_value) == true) {
                String value = o.substring(hyphen_option_value.length());

                try {
                    return Long.parseLong(value);
                } catch (final NumberFormatException e) {
                    MengineLog.logSingleWarning(TAG, "option [%s] invalid long value [%s]", option, value);

                    return defaultValue;
                }
            }

            if (o.startsWith(double_hyphen_option_value) == true) {
                String value = o.substring(double_hyphen_option_value.length());

                try {
                    return Long.parseLong(value);
                } catch (final NumberFormatException e) {
                    MengineLog.logSingleWarning(TAG, "option [%s] invalid long value [%s]", option, value);

                    return defaultValue;
                }
            }
        }

        return defaultValue;
    }

    public String getOptionValueString(String option, String defaultValue) {
        if (BuildConfig.DEBUG == false) {
            return defaultValue;
        }

        String options_str = this.getApplicationOptions();

        List<String> options = Splitter.on(' ').splitToList(options_str);

        if (options.isEmpty() == true) {
            return defaultValue;
        }

        String hyphen_option = "-" + option;
        String double_hyphen_option = "--" + option;

        String hyphen_option_value = hyphen_option + ":";
        String double_hyphen_option_value = double_hyphen_option + ":";

        for(String o : options) {
            if (o.equals(hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.equals(double_hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.startsWith(hyphen_option_value) == true) {
                String value = o.substring(hyphen_option_value.length());
                return value;
            }

            if (o.startsWith(double_hyphen_option_value) == true) {
                String value = o.substring(double_hyphen_option_value.length());
                return value;
            }
        }

        return defaultValue;
    }

    public String getAndroidId() {
        return m_androidId;
    }

    public long getSaveVersion() {
        return m_saveVersion;
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

        MenginePreferences.setPreferenceString(this, TAG, "session_id", m_sessionId);

        this.setState("user.session_id", m_sessionId);

        List<MengineListenerSessionId> listeners = this.getSessionIdListeners();

        for (MengineListenerSessionId l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineSetSessionId(this, m_sessionId);
        }
    }

    public void removeSessionData() {
        MenginePreferences.clearPreferences(this, TAG);

        List<MengineListenerSessionId> listeners = this.getSessionIdListeners();

        for (MengineListenerSessionId l : listeners) {
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

    public void setState(@NonNull @Size(min = 1L,max = 1024L) String name, Object value) {
        m_states.put(name, value);

        List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

        synchronized (m_syncState) {
            for (MengineListenerApplication listener : applicationListeners) {
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

    public List<MengineService> getServices() {
        return m_services;
    }

    public List<MengineListenerLogger> getLoggerListeners() {
        return m_loggerListeners;
    }

    public List<MengineListenerAnalytics> getAnalyticsListeners() {
        return m_analyticsListeners;
    }

    public List<MengineListenerAdRevenue> getAdRevenueListeners() {
        return m_adRevenueListeners;
    }

    public List<MengineListenerTransparencyConsent> getTransparencyConsentListeners() {
        return m_transparencyConsentListeners;
    }

    public List<MengineListenerInAppPurchase> getInAppAnalyticsListeners() {
        return m_inAppPurchaseListeners;
    }

    public List<MengineListenerRemoteMessage> getRemoteMessageListeners() {
        return m_removeMessageListeners;
    }

    public List<MengineListenerKeyEvent> getKeyListeners() {
        return m_keyListeners;
    }

    public List<MengineListenerApplication> getApplicationListeners() {
        return m_applicationListeners;
    }

    public List<MengineListenerActivity> getActivityListeners() {
        return m_activityListeners;
    }

    public List<MengineListenerEngine> getEngineListeners() {
        return m_engineListeners;
    }

    public List<MengineListenerPushToken> getPushTokenListeners() {
        return m_pushTokenListeners;
    }

    public List<MengineListenerAdvertisingId> getAdvertisingIdListeners() {
        return m_advertisingIdListeners;
    }

    public List<MengineListenerSessionId> getSessionIdListeners() {
        return m_sessionIdListeners;
    }

    public List<MengineListenerPerformance> getPerformanceListeners() {
        return m_performanceListeners;
    }

    public List<MengineListenerRemoteConfig> getRemoteConfigListeners() {
        return m_remoteConfigListeners;
    }

    protected void explainServiceListeners(MengineServiceInterface service) {
        if (service instanceof MengineListenerAnalytics listener) {
            m_analyticsListeners.add(listener);
        }

        if (service instanceof MengineListenerAdRevenue listener) {
            m_adRevenueListeners.add(listener);
        }

        if (service instanceof MengineListenerTransparencyConsent listener) {
            m_transparencyConsentListeners.add(listener);
        }

        if (service instanceof MengineListenerInAppPurchase listener) {
            m_inAppPurchaseListeners.add(listener);
        }

        if (service instanceof MengineListenerRemoteMessage listener) {
            m_removeMessageListeners.add(listener);
        }

        if (service instanceof MengineListenerLogger listener) {
            m_loggerListeners.add(listener);
        }

        if (service instanceof MengineListenerKeyEvent listener) {
            m_keyListeners.add(listener);
        }

        if (service instanceof MengineListenerApplication listener) {
            m_applicationListeners.add(listener);
        }

        if (service instanceof MengineListenerActivity listener) {
            m_activityListeners.add(listener);
        }

        if (service instanceof MengineListenerEngine listener) {
            m_engineListeners.add(listener);
        }

        if (service instanceof MengineListenerPushToken listener) {
            m_pushTokenListeners.add(listener);
        }

        if (service instanceof MengineListenerAdvertisingId listener) {
            m_advertisingIdListeners.add(listener);
        }

        if (service instanceof MengineListenerSessionId listener) {
            m_sessionIdListeners.add(listener);
        }

        if (service instanceof MengineListenerPerformance listener) {
            m_performanceListeners.add(listener);
        }

        if (service instanceof MengineListenerRemoteConfig listener) {
            m_remoteConfigListeners.add(listener);
        }
    }

    static protected String getServiceClassName(Class<?> cls) {
        String name;

        try {
            Field SERVICE_NAME = cls.getField("SERVICE_NAME");

            name = (String)SERVICE_NAME.get(null);
        } catch (final NoSuchFieldException ex) {
            MengineLog.logError(TAG, "[ERROR] service [%s] not found field SERVICE_NAME" );

            return null;
        } catch (IllegalAccessException ex) {
            MengineLog.logError(TAG, "[ERROR] service [%s] invalid field SERVICE_NAME" );

            return null;
        }

        if (name == null) {
            MengineLog.logError(TAG, "[ERROR] service [%s] invalid name" );

            return null;
        }

        if (name.length() > 23) {
            MengineLog.logError(TAG, "[ERROR] service [%s] invalid name: %s [length > 23]"
                , name
            );

            return null;
        }

        return name;
    }

    static protected boolean getServiceClassNameEmbedding(MengineService service, Class<?> cls) {
        try {
            Field SERVICE_EMBEDDING = cls.getField("SERVICE_EMBEDDING");

            if (SERVICE_EMBEDDING.getBoolean(service) == false) {
                return false;
            }
        } catch (final NoSuchFieldException e) {
            return false;
        } catch (final IllegalAccessException e) {
            return false;
        }

        return true;
    }

    public MengineService findService(String name) {
        MengineService service = m_dictionaryServices.get(name);

        return service;
    }

    @SuppressWarnings("unchecked")
    public <T> T getService(Class<T> cls) {
        String name = MengineApplication.getServiceClassName(cls);

        if (name == null) {
            return null;
        }

        MengineService service = this.findService(name);

        return (T)service;
    }

    protected boolean createService(String type) {
        MengineService service = (MengineService)MengineUtils.newInstance(TAG, type, true);

        if (service == null) {
            MengineLog.logError(TAG, "invalid create instance service: %s"
                , type
            );

            return false;
        }

        Class<?> cls = service.getClass();

        String name = MengineApplication.getServiceClassName(cls);

        if (name == null) {
            return false;
        }

        boolean embedding = MengineApplication.getServiceClassNameEmbedding(service, cls);

        if (service.onInitialize(this, name, embedding) == false) {
            MengineLog.logError(TAG, "[ERROR] service [%s] invalid initialize"
                , type
            );

            return false;
        }

        this.explainServiceListeners(service);

        m_services.add(service);
        m_dictionaryServices.put(name, service);

        MengineLog.logMessage(TAG, "create service: %s [%s]"
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

        List<MengineListenerEngine> listeners = this.getEngineListeners();

        for (MengineListenerEngine l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineCaughtException(this, throwable);
        }
    }

    public void sendEvent(MengineEvent event, Object ... args) {
        List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

        synchronized (m_syncEvent) {
            for (MengineListenerApplication listener : applicationListeners) {
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
        String installKey = "MNIK" + MengineUtils.getSecureRandomHexString(16).toUpperCase(Locale.US);

        return installKey;
    }

    protected Object createNativeApplication() {
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

            return null;
        }

        return nativeApplication;
    }

    protected void destroyNativeApplication() {
        if (m_nativeApplication == null) {
            return;
        }

        MengineNative.AndroidMain_destroy(m_nativeApplication);
        m_nativeApplication = null;
    }

    protected Object getNativeApplication() {
        return m_nativeApplication;
    }

    @Override
    public void onCreate() {
        super.onCreate();

        if (m_invalidInitialize == true) {
            return;
        }

        boolean isMainProcess = this.isMainProcess();

        MengineLog.logMessageRelease(TAG, "onCreate isMainProcess: %b "
            , isMainProcess
        );

        List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

        for (MengineListenerApplication l : applicationListeners) {
            try {
                if (l.onAvailable(this) == false) {
                    continue;
                }

                MengineLog.logInfo(TAG, "onAppInit service: %s isMainProcess: %b"
                    , l.getServiceName()
                    , isMainProcess
                );

                l.onAppInit(this, isMainProcess);
            } catch (final MengineServiceInvalidInitializeException e) {
                this.invalidInitialize("onAppInit service: %s exception: %s"
                    , l.getServiceName()
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

        this.setState("application.init", "load_preferences");

        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(this, TAG);

        long MENGINE_APPLICATION_SAVE_VERSION = 1L;

        long saveVersion = settings.getLong("save_version", MENGINE_APPLICATION_SAVE_VERSION);
        String installKey = settings.getString("install_key", null);
        long installTimestamp = settings.getLong("install_timestamp", -1);
        String installVersion = settings.getString("install_version", "");
        long installRND = settings.getLong("install_rnd", -1);
        long sessionIndex = settings.getLong("session_index", 0);
        String sessionId = settings.getString("session_id", null);

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

        m_saveVersion = saveVersion;
        m_installKey = installKey;
        m_installTimestamp = installTimestamp;
        m_installVersion = installVersion;
        m_installRND = installRND;
        m_sessionIndex = sessionIndex;
        m_sessionId = sessionId;
        m_sessionTimestamp = MengineUtils.getTimestamp();

        MengineStatistic.load(this);

        this.setState("user.save_version", m_saveVersion);
        this.setState("user.install_key", m_installKey);
        this.setState("user.install_timestamp", m_installTimestamp);
        this.setState("user.install_version", m_installVersion);
        this.setState("user.install_rnd", m_installRND);
        this.setState("user.session_index", m_sessionIndex);
        this.setState("user.session_id", m_sessionId);
        this.setState("user.session_timestamp", m_sessionTimestamp);

        MengineAnalytics.addContextParameterBoolean("is_dev", BuildConfig.DEBUG);
        MengineAnalytics.addContextParameterString("install_key", m_installKey);
        MengineAnalytics.addContextParameterLong("install_timestamp", m_installTimestamp);
        MengineAnalytics.addContextParameterString("install_version", m_installVersion);
        MengineAnalytics.addContextParameterLong("install_rnd", m_installRND);
        MengineAnalytics.addContextParameterLong("session_index", m_sessionIndex);
        MengineAnalytics.addContextParameterLong("session_timestamp", m_sessionTimestamp);

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

        ClassLoader cl = MengineApplication.class.getClassLoader();

        MengineNative.AndroidEnv_setMengineAndroidApplicationJNI(this, cl);

        String engine_gitsha1 = this.getEngineGITSHA1();
        this.setState("engine.gitsha1", engine_gitsha1);

        this.setState("application.init", "services_version");

        Map<String, String> pluginVersions = new HashMap<>();

        for (MengineListenerApplication l : applicationListeners) {
            String version = l.onAppVersion(this);

            if (version == null) {
                continue;
            }

            String serviceName = l.getServiceName();

            pluginVersions.put(serviceName, version);
        }

        for (Map.Entry<String, String> entry: pluginVersions.entrySet()) {
            String name = entry.getKey();
            String version = entry.getValue();

            MengineLog.logMessage(TAG, "plugin: %s version: %s"
                , name
                , version
            );
        }

        this.setState("application.init", "services_prepare");

        for (MengineListenerApplication l : applicationListeners) {
            try {
                MengineLog.logInfo(TAG, "onAppPrepare service: %s"
                    , l.getServiceName()
                );

                l.onAppPrepare(this, pluginVersions);
            } catch (final MengineServiceInvalidInitializeException e) {
                MengineAnalytics.buildEvent("mng_app_create_failed")
                    .addParameterException("exception", e)
                    .logAndFlush();

                this.invalidInitialize("onAppPrepare service: %s exception: %s"
                    , l.getServiceName()
                    , e.getMessage()
                );

                return;
            }
        }

        this.setState("application.init", "services_load");

        this.onServicesLoad();

        this.setState("application.init", "services_create");

        for (MengineListenerApplication l : applicationListeners) {
            try {
                String serviceName = l.getServiceName();

                MengineLog.logInfo(TAG, "onAppCreate service: %s"
                    , serviceName
                );

                l.onAppCreate(this);
            } catch (MengineServiceInvalidInitializeException e) {
                MengineAnalytics.buildEvent("mng_app_create_failed")
                    .addParameterException("exception", e)
                    .logAndFlush();

                this.invalidInitialize("onAppCreate service: %s exception: %s"
                    , l.getServiceName()
                    , e.getMessage()
                );

                return;
            }
        }

        this.setState("application.init", "services_post");

        for (MengineListenerApplication l : applicationListeners) {
            try {
                String serviceName = l.getServiceName();

                MengineLog.logInfo(TAG, "onAppPost service: %s"
                    , serviceName
                );

                l.onAppPost(this);
            } catch (MengineServiceInvalidInitializeException e) {
                MengineAnalytics.buildEvent("mng_app_create_failed")
                    .addParameterException("exception", e)
                    .logAndFlush();

                this.invalidInitialize("onAppPost service: %s exception: %s"
                    , l.getServiceName()
                    , e.getMessage()
                );

                return;
            }
        }

        Object nativeApplication = this.createNativeApplication();

        if (nativeApplication == null) {
            return;
        }

        m_nativeApplication = nativeApplication;

        for (MengineService p : m_services) {
            if (p.onAvailable(this) == false) {
                continue;
            }

            if (p.isEmbedding() == false) {
                continue;
            }

            String name = p.getServiceName();

            MengineNative.AndroidNativePython_addPlugin("Mengine" + name, p);
        }

        this.setState("application.init", "run_main");

        MengineNative.AndroidNativePython_addPlugin("Application", this);

        m_main = new MengineMain(m_nativeApplication);
        m_main.start();

        this.setState("application.init", "completed");
    }

    public void onServicesLoad() {
        MengineLog.logInfo(TAG, "onServicesLoad");

        List<MengineService> services = this.getServices();

        for (MengineService s : services) {
            String serviceName = s.getServiceName();

            Bundle bundle = MenginePreferences.getPreferenceBundle(this, TAG, "service." + serviceName);

            if (bundle == null) {
                continue;
            }

            MengineLog.logInfo(TAG, "onLoad service: %s bundle: %s"
                , serviceName
                , bundle
            );

            s.onLoad(this, bundle);
        }
    }

    public void onServicesSave() {
        MengineLog.logInfo(TAG, "onServicesSave");

        List<MengineService> services = this.getServices();

        for (MengineService s : services) {
            String serviceName = s.getServiceName();

            Bundle bundle;

            try {
                bundle = s.onSave(this);
            } catch (final Exception e) {
                MengineLog.logError(TAG, "onSave service: %s exception: %s"
                    , serviceName
                    , e.getMessage()
                );

                continue;
            }

            if (bundle == null) {
                continue;
            }

            this.forceSaveService(s, bundle);
        }
    }

    public void forceSaveService(@NonNull MengineService s, Bundle bundle) {
        String serviceName = s.getServiceName();

        if (BuildConfig.DEBUG == true) {
            if (bundle.containsKey("version") == false) {
                MengineLog.logFatal(TAG, "onSave service: %s missing version"
                    , serviceName
                );

                return;
            }
        }

        MengineLog.logInfo(TAG, "onSave service: %s bundle: %s"
            , serviceName
            , bundle
        );

        MenginePreferences.setPreferenceBundle(this, TAG, "service." + serviceName, bundle);
    }

    @Override
    public void onTerminate() {
        MengineLog.logMessage(TAG, "onTerminate");

        this.onServicesSave();

        List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

        for (MengineListenerApplication l : applicationListeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onAppTerminate(this);
        }

        for (MengineService p : m_services) {
            if (p.onAvailable(this) == false) {
                continue;
            }

            if (p.isEmbedding() == false) {
                continue;
            }

            String name = p.getServiceName();

            MengineNative.AndroidNativePython_removePlugin("Mengine" + name);
        }

        for (MengineService service : m_services) {
            service.onFinalize(this);
        }

        MengineNative.AndroidNativePython_removePlugin("Application");

        m_main.stop();
        m_main = null;

        this.destroyNativeApplication();

        MengineNative.AndroidEnv_removeMengineAndroidApplicationJNI();

        super.onTerminate();
    }

    @Override
    public void attachBaseContext(Context base) {
        super.attachBaseContext(base);

        MengineLog.logMessage(TAG, "attachBaseContext");

        List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

        for (MengineListenerApplication l : applicationListeners) {
            l.onAppAttachBaseContext(this, base);
        }
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        MengineLog.logMessage(TAG, "onConfigurationChanged config: %s"
            , newConfig.toString()
        );

        List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

        for (MengineListenerApplication l : applicationListeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onAppConfigurationChanged(this, newConfig);
        }
    }

    public void onMengineInitializeBaseServices() {
        MengineLog.logInfo(TAG, "onMengineInitializeBaseServices");

        m_isMengineInitializeBaseServices = true;

        List<MengineListenerEngine> listeners = this.getEngineListeners();

        for (MengineListenerEngine l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineInitializeBaseServices(this);
        }
    }

    public void onMengineCreateApplication() {
        MengineLog.logInfo(TAG, "onMengineCreateApplication");

        m_isMengineCreateApplication = true;

        List<MengineListenerEngine> listeners = this.getEngineListeners();

        for (MengineListenerEngine l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineCreateApplication(this);
        }
    }

    public void onMenginePlatformRun() {
        MengineLog.logInfo(TAG, "onMenginePlatformRun");

        m_isMenginePlatformRun = true;

        List<MengineListenerEngine> listeners = this.getEngineListeners();

        for (MengineListenerEngine l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMenginePlatformRun(this);
        }
    }

    public void onMenginePlatformStop() {
        MengineLog.logInfo(TAG, "onMenginePlatformStop");

        m_isMenginePlatformRun = false;

        List<MengineListenerEngine> listeners = this.getEngineListeners();

        for (MengineListenerEngine l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMenginePlatformStop(this);
        }
    }

    public void onMengineLogger(int level, int filter, String category, String msg) {
        List<MengineListenerLogger> listeners = this.getLoggerListeners();

        for(MengineListenerLogger l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineLogger(this, level, filter, category, msg);
        }

        if (BuildConfig.DEBUG == true) {
            if (m_activity != null) {
                switch (level) {
                    case MengineLog.LM_FATAL:
                        MengineUtils.showOkAlertDialog(m_activity, () -> {
                            System.exit(0);
                        }, "Fatal", "%s", msg);
                        break;
                    case MengineLog.LM_ERROR:
                        MengineUtils.showToast(m_activity, "%s", msg);
                        break;
                }
            }
        }
    }

    public void onMengineAnalyticsEvent(@NonNull @Size(min = 1L,max = 40L) String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        List<MengineListenerAnalytics> listeners = this.getAnalyticsListeners();

        for (MengineListenerAnalytics l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineAnalyticsEvent(this, eventName, timestamp, bases, parameters);
        }
    }

    public void onMengineAnalyticsScreenView(@NonNull @Size(min = 1L,max = 40L) String screenType, @NonNull @Size(min = 0L,max = 100L) String screenName) {
        MengineLog.logInfo(TAG, "onMengineAnalyticsScreenView screenType: %s screenName: %s"
            , screenType
            , screenName
        );

        this.setState("current.screen", screenType + "." + screenName);

        List<MengineListenerAnalytics> listeners = this.getAnalyticsListeners();

        for (MengineListenerAnalytics l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineAnalyticsScreenView(this, screenType, screenName);
        }
    }

    public void onMengineAnalyticsFlush() {
        List<MengineListenerAnalytics> listeners = this.getAnalyticsListeners();

        for (MengineListenerAnalytics l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineAnalyticsFlush(this);
        }
    }

    public void onMengineRemoteConfigFetch(@NonNull Map<String, JSONObject> configs) {
        List<MengineListenerRemoteConfig> listeners = this.getRemoteConfigListeners();

        for (MengineListenerRemoteConfig l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineRemoteConfigFetch(this, configs);
        }
    }

    public void onMengineAdRevenue(MengineAdRevenueParam revenue) {
        List<MengineListenerAdRevenue> listeners = this.getAdRevenueListeners();

        for (MengineListenerAdRevenue l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineAdRevenue(this, revenue);
        }
    }

    public void onMengineTransparencyConsent(MengineTransparencyConsentParam tcParam) {
        List<MengineListenerTransparencyConsent> listeners = this.getTransparencyConsentListeners();

        for (MengineListenerTransparencyConsent l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineTransparencyConsent(this, tcParam);
        }
    }

    public void onMengineInAppProducts(List<MengineInAppProductParam> products) {
        List<MengineListenerInAppPurchase> listeners = this.getInAppAnalyticsListeners();

        for (MengineListenerInAppPurchase l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineInAppProduct(this, products);
        }
    }

    public void onMengineInAppPurchase(MengineInAppPurchaseParam purchase) {
        long purchase_timestamp = MengineUtils.getTimestamp();

        MengineStatistic.setInteger(this, "purchase.timestamp", purchase_timestamp);

        List<MengineListenerInAppPurchase> listeners = this.getInAppAnalyticsListeners();

        for (MengineListenerInAppPurchase l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineInAppPurchase(this, purchase);
        }
    }

    public void onMengineRemoteMessageReceived(MengineRemoteMessageParam message) {
        List<MengineListenerRemoteMessage> listeners = this.getRemoteMessageListeners();

        for (MengineListenerRemoteMessage l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            if (l.onMengineRemoteMessageReceived(this, message) == true) {
                break;
            }
        }
    }

    public void onMengineRemoteMessageDeleted() {
        List<MengineListenerRemoteMessage> listeners = this.getRemoteMessageListeners();

        for (MengineListenerRemoteMessage l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineRemoteMessageDeleted(this);
        }
    }

    public void onMengineRemoteMessageSent(String messageId) {
        List<MengineListenerRemoteMessage> listeners = this.getRemoteMessageListeners();

        for (MengineListenerRemoteMessage l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineRemoteMessageSent(this, messageId);
        }
    }

    public void onMengineRemoteMessageSentError(String messageId, Exception exception) {
        List<MengineListenerRemoteMessage> listeners = this.getRemoteMessageListeners();

        for (MengineListenerRemoteMessage l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineRemoteMessageSentError(this, messageId, exception);
        }
    }

    public void onMengineRemoteMessageNewToken(String token) {
        List<MengineListenerRemoteMessage> listeners = this.getRemoteMessageListeners();

        for (MengineListenerRemoteMessage l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineRemoteMessageNewToken(this, token);
        }
    }

    public void onMenginePushToken(String token) {
        List<MengineListenerPushToken> listeners = this.getPushTokenListeners();

        for (MengineListenerPushToken l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMenginePushToken(this, token);
        }
    }

    public void onMengineAdvertisingId(String advertisingId, boolean limitAdTracking) {
        this.setState("user.limit_ad_tracking", limitAdTracking);

        List<MengineListenerAdvertisingId> listeners = this.getAdvertisingIdListeners();

        for (MengineListenerAdvertisingId l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMengineAdvertisingId(this, advertisingId, limitAdTracking);
        }
    }

    public MenginePerformanceTrace startPerformanceTrace(String name, Map<String, String> attributes) {
        MenginePerformanceTrace trace = new MenginePerformanceTrace();

        List<MengineListenerPerformance> listeners = this.getPerformanceListeners();

        for (MengineListenerPerformance l : listeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onMenginePerformanceTraceStart(this, trace, name, attributes);
        }

        return trace;
    }

    private void invalidInitialize(String format, Object ... args) {
        String msg = MengineLog.logErrorException(TAG, format, args);

        m_invalidInitialize = true;
        m_invalidInitializeReason = msg;
    }
}