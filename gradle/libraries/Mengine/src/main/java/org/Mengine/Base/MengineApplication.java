package org.Mengine.Base;

import android.app.Application;
import android.content.ContentResolver;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.os.Build;
import android.os.Bundle;
import android.os.StrictMode;
import android.provider.Settings;

import androidx.annotation.AnyRes;
import androidx.annotation.BoolRes;
import androidx.annotation.IntegerRes;
import androidx.annotation.NonNull;
import androidx.annotation.Size;
import androidx.annotation.StringRes;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

public abstract class MengineApplication extends Application {
    private static final MengineTag TAG = MengineTag.of("MengineApplication");

    static {
        System.loadLibrary("AndroidApplication");
    }

    public static MengineApplication INSTANCE = null;

    private String m_androidId;

    private long m_saveVersion = -1;
    private String m_installId;
    private long m_installTimestamp = -1;
    private String m_installVersion;
    private long m_installRND = -1;
    private long m_sessionIndex = -1;
    private long m_sessionTimestamp = -1;
    private long m_sessionRND = -1;
    private String m_sessionId;

    protected String m_userId;

    //ToDo sync acquisition
    protected String m_adid;
    protected String m_acquisitionNetwork;
    protected String m_acquisitionCampaign;

    private boolean m_invalidInitialize = false;
    private Throwable m_invalidInitializeException = null;

    private MengineMain m_main;

    private Object m_nativeApplication;

    private final List<MengineServiceInterface> m_services = new ArrayList<>();
    private final Map<String, MengineService> m_dictionaryServices = new HashMap<>();
    private final Map<String, Object> m_states = new ConcurrentHashMap<>();

    private static final Object m_syncronizationSemaphores = new Object();
    private final Map<String, MengineSemaphore> m_semaphores = new HashMap<>();

    private final List<MengineFragmentInterface> m_fragments = new ArrayList<>();

    private final List<MengineListenerApplication> m_applicationListeners = new ArrayList<>();

    private final Object m_syncState = new Object();

    public static boolean isMasterRelease() {
        return MengineNative.AndroidEnv_isMasterRelease();
    }

    public static String getEngineGITSHA1() {
        return MengineNative.AndroidEnv_getEngineGITSHA1();
    }

    public static String getEngineVersion() {
        return MengineNative.AndroidEnv_getEngineVersion();
    }

    public static String getBuildDate() {
        return MengineNative.AndroidEnv_getBuildDate();
    }

    public static String getBuildUsername() {
        return MengineNative.AndroidEnv_getBuildUsername();
    }

    public abstract String[] getAndroidPlugins();
    public abstract String[] getAndroidActivities();
    public abstract String getApplicationId();
    public abstract int getVersionCode();
    public abstract String getVersionName();
    public abstract boolean isBuildPublish();
    public abstract String getApplicationOptions();

    public void createFragment(Class<?> cls) {
        MengineFragmentInterface fragment = (MengineFragmentInterface) MengineUtils.newInstance(TAG, cls, true);

        if (fragment == null) {
            MengineLog.logError(TAG, "invalid create instance fragment: %s"
                , cls.getName()
            );

            return;
        }

        m_fragments.add(fragment);
    }

    public void createProcedure(Class<? extends MengineProcedureInterface> cls) {
        try {
            Class.forName(cls.getName(), true, cls.getClassLoader());
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }
    }

    public MengineApplication() {
        super();

        MengineApplication.INSTANCE = this;

        this.createFragment(MengineFragmentAdRevenue.class);
        this.createFragment(MengineFragmentAcquisition.class);
        this.createFragment(MengineFragmentAdvertisingId.class);
        this.createFragment(MengineFragmentAnalytics.class);
        this.createFragment(MengineFragmentEngine.class);
        this.createFragment(MengineFragmentGame.class);
        this.createFragment(MengineFragmentInAppPurchase.class);
        this.createFragment(MengineFragmentKeyEvent.class);
        this.createFragment(MengineFragmentLogger.class);
        this.createFragment(MengineFragmentPerformance.class);
        this.createFragment(MengineFragmentPushToken.class);
        this.createFragment(MengineFragmentRemoteConfig.class);
        this.createFragment(MengineFragmentRemoteMessage.class);
        this.createFragment(MengineFragmentUser.class);
        this.createFragment(MengineFragmentTransparencyConsent.class);

        this.createProcedure(MengineProcedureOpenUrl.class);
        this.createProcedure(MengineProcedureSendMail.class);
        this.createProcedure(MengineProcedureDeleteAccount.class);

        String applicationId = this.getApplicationId();
        String versionName = this.getVersionName();
        int versionCode = this.getVersionCode();

        MengineLog.logInfo(TAG, "MengineApplication id: %s version: %s [%d]"
            , applicationId
            , versionName
            , versionCode
        );

        String[] plugins = this.getAndroidPlugins();

        for (String namePlugin : plugins) {
            if (this.createService(namePlugin) == false) {
                this.invalidInitialize(new MengineServiceInvalidInitializeException("invalid create plugin"), Map.of("service", namePlugin));

                return;
            }
        }
    }

    public String getAndroidId() {
        return m_androidId;
    }

    public long getSaveVersion() {
        return m_saveVersion;
    }

    public String getInstallId() {
        return m_installId;
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

    public long getSessionTimestamp() {
        return m_sessionTimestamp;
    }

    public long getSessionRND() {
        return m_sessionRND;
    }

    public String getSessionId() {
        return m_sessionId;
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

    public void setState(@NonNull @Size(min = 1L,max = 1024L) String name, Object value) {
        MengineLog.logDebug(TAG, "setState: %s = %s"
            , name
            , value
        );

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

    public List<MengineServiceInterface> getServices() {
        return m_services;
    }

    public List<MengineListenerApplication> getApplicationListeners() {
        return m_applicationListeners;
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

    static protected boolean getServiceClassEmbedding(Class<?> cls) {
        try {
            Field SERVICE_EMBEDDING = cls.getField("SERVICE_EMBEDDING");

            if (SERVICE_EMBEDDING.getBoolean(null) == false) {
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
        ClassLoader cl = MengineApplication.class.getClassLoader();

        MengineService service = (MengineService)MengineUtils.newInstance(cl, TAG, type, true);

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

        boolean embedding = MengineApplication.getServiceClassEmbedding(cls);

        service.onAppInitialize(this, name, embedding);

        for (MengineFragmentInterface fragment : m_fragments) {
            fragment.explainServiceListeners(service);
        }

        if (service instanceof MengineListenerApplication listener) {
            m_applicationListeners.add(listener);
        }

        m_services.add(service);
        m_dictionaryServices.put(name, service);

        MengineLog.logMessage(TAG, "create service: %s [%s]"
            , type
            , name
        );

        return true;
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

        if (Objects.equals(packageName, processName) == false) {
            return false;
        }

        return true;
    }

    public MengineParamTransparencyConsent makeTransparencyConsentParam() {
        MengineParamTransparencyConsent tcParam = new MengineParamTransparencyConsent();

        Context context = this.getApplicationContext();
        tcParam.initFromDefaultSharedPreferences(context);

        return tcParam;
    }

    public void checkTransparencyConsentServices() {
        MengineParamTransparencyConsent tcParam = this.makeTransparencyConsentParam();
        MengineFragmentTransparencyConsent.INSTANCE.transparencyConsent(tcParam);
    }

    public boolean isInvalidInitialize() {
        return m_invalidInitialize;
    }

    public Throwable getInvalidInitializeException() {
        return m_invalidInitializeException;
    }

    protected Object createNativeApplication() {
        MengineLog.logInfo(TAG, "[BEGIN] bootstrap native application");

        ApplicationInfo applicationInfo = this.getApplicationInfo();
        String nativeLibraryDir = applicationInfo.nativeLibraryDir;
        String options = this.getApplicationOptions();

        String[] optionsArgs = options.split(" ");

        Object nativeApplication = MengineNative.AndroidMain_bootstrap(nativeLibraryDir, optionsArgs);

        if (nativeApplication == null) {
            MengineAnalytics.buildEvent("mng_app_create_failed")
                .addParameterString("reason", "bootstrap failed")
                .logAndFlush();

            this.invalidInitialize(new MengineServiceInvalidInitializeException("bootstrap failed"), Map.of());

            return null;
        }

        MengineLog.logInfo(TAG, "[END] bootstrap native application");

        return nativeApplication;
    }

    protected void destroyNativeApplication() {
        if (m_nativeApplication == null) {
            return;
        }

        MengineLog.logInfo(TAG, "[BEGIN] destroy native application");

        MengineNative.AndroidMain_destroy(m_nativeApplication);
        m_nativeApplication = null;

        MengineLog.logInfo(TAG, "[END] destroy native application");
    }

    protected Object getNativeApplication() {
        return m_nativeApplication;
    }

    public void setUserId(String userId) {
        if (Objects.equals(m_userId, userId) == true) {
            return;
        }

        String old_userId = m_userId;

        m_userId = userId;

        MenginePreferences.setPreferenceString("user_id", m_userId);

        MengineFragmentUser.INSTANCE.changeUserId(old_userId, userId);
    }

    public String getUserId() {
        return m_userId;
    }

    public void removeUserData() {
        MenginePreferences.clearPreferences();

        MengineFragmentUser.INSTANCE.removeUserData();

        MengineNative.AndroidEnvironmentService_deleteCurrentAccount();
    }

    public void setADID(String adid) {
        if (Objects.equals(m_adid, adid) == true) {
            return;
        }

        m_adid = Objects.requireNonNullElse(adid, "");

        MenginePreferences.setPreferenceString("adid", m_adid);

        MengineFragmentAcquisition.INSTANCE.changeADID(m_adid);
    }

    public String getADID() {
        return m_adid;
    }

    public void setAcquisitionCampaign(String network, String campaign) {
        if (Objects.equals(m_acquisitionNetwork, network) == true && Objects.equals(m_acquisitionCampaign, campaign) == true) {
            return;
        }

        m_acquisitionNetwork = Objects.requireNonNullElse(network, "");
        m_acquisitionCampaign = Objects.requireNonNullElse(campaign, "");

        this.setState("user.acquisition_network", m_acquisitionNetwork);
        this.setState("user.acquisition_campaign", m_acquisitionCampaign);

        MenginePreferences.setPreferenceString("acquisition_network", m_acquisitionNetwork);
        MenginePreferences.setPreferenceString("acquisition_campaign", m_acquisitionCampaign);

        MengineParamAcquisition acquisition = new MengineParamAcquisition();
        acquisition.ACQUISITION_NETWORK = m_acquisitionNetwork;
        acquisition.ACQUISITION_CAMPAIGN = m_acquisitionCampaign;

        MengineFragmentAcquisition.INSTANCE.changeAcquisition(acquisition);
    }

    public String getAcquisitionNetwork() {
        return m_acquisitionNetwork;
    }

    public String getAcquisitionCampaign() {
        return m_acquisitionCampaign;
    }

    public String getResourceName(@AnyRes int resId) {
        Resources resources = this.getResources();

        String name = resources.getResourceEntryName(resId);

        return name;
    }

    public boolean getResourceBoolean(@BoolRes int resId) {
        Resources resources = this.getResources();

        boolean value = resources.getBoolean(resId);

        return value;
    }

    public int getResourceInteger(@IntegerRes int resId) {
        Resources resources = this.getResources();

        int value = resources.getInteger(resId);

        return value;
    }

    public String getResourceString(@StringRes int resId) {
        Resources resources = this.getResources();

        String value = resources.getString(resId);

        return value;
    }

    @Override
    public void onCreate() {
        MengineLog.logInfo(TAG, "[BEGIN] onCreate");

        if (m_invalidInitialize == false) {
            List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

            for (MengineListenerApplication l : applicationListeners) {
                try {
                    if (l.onAvailable(this) == false) {
                        continue;
                    }

                    MengineLog.logInfo(TAG, "onAppInit service: %s"
                        , l.getServiceName()
                    );

                    l.onAppPreInit(this);
                } catch (final MengineServiceInvalidInitializeException e) {
                    this.invalidInitialize(e, Map.of("service", l.getServiceName()));

                    return;
                }
            }
        }

        super.onCreate();

        if (m_invalidInitialize == true) {
            MengineLog.logError(TAG, "[ERROR] invalid initialize application, skip onCreate");

            return;
        }

        boolean isMainProcess = this.isMainProcess();

        MengineLog.logDebug(TAG, "isMainProcess: %b"
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
                this.invalidInitialize(e, Map.of("service", l.getServiceName()));

                return;
            }
        }

        if (isMainProcess == false) {
            return;
        }

        if (BuildConfig.DEBUG == true) {
            String deviceInfo = MengineUtils.getPrintDeviceInfo();

            MengineLog.logMessage(TAG, deviceInfo);
        }

        if (BuildConfig.MENGINE_APP_ENABLE_STRICT_MODE == true) {
            StrictMode.setThreadPolicy(
                new StrictMode.ThreadPolicy.Builder()
                    .detectAll()
                    .penaltyLog()
                    .build());

            StrictMode.setVmPolicy(
                new StrictMode.VmPolicy.Builder()
                    .detectAll()
                    .penaltyLog()
                    .build());
        }

        long sessionTimestamp = MengineUtils.getTimestamp();

        this.setState("build.debug", BuildConfig.DEBUG);

        this.setState("application.init", "started");

        this.setState("application.init", "load_preferences");

        MenginePreferences.initialize(this, TAG);

        long MENGINE_APPLICATION_SAVE_VERSION = 1L;

        long saveVersion = MenginePreferences.getPreferenceLong("save_version", MENGINE_APPLICATION_SAVE_VERSION);
        String old_installId = MenginePreferences.getPreferenceString("install_id", null); //Deprecated
        String installId = MenginePreferences.getPreferenceString("install_id", old_installId);
        long installTimestamp = MenginePreferences.getPreferenceLong("install_timestamp", -1);
        String installVersion = MenginePreferences.getPreferenceString("install_version", "");
        long installRND = MenginePreferences.getPreferenceLong("install_rnd", -1);
        long sessionIndex = MenginePreferences.getPreferenceLong("session_index", 0);

        String old_userId = MenginePreferences.getPreferenceString("session_id", null); //Deprecated
        String userId = MenginePreferences.getPreferenceString("user_id", old_userId);

        String adid = MenginePreferences.getPreferenceString("adid", "");
        String acquisitionNetwork = MenginePreferences.getPreferenceString("acquisition_network", "");
        String acquisitionCampaign = MenginePreferences.getPreferenceString("acquisition_campaign", "");

        Bundle preferences = new Bundle();

        preferences.putLong("save_version", MENGINE_APPLICATION_SAVE_VERSION);

        if (installId == null) {
            installId = (BuildConfig.DEBUG == true ? "MDID" : "MRID") + MengineUtils.getSecureRandomHexString(32).toUpperCase(Locale.US);
            installTimestamp = MengineUtils.getTimestamp();
            installVersion = this.getVersionName();

            preferences.putString("install_id", installId);
            preferences.putLong("install_timestamp", installTimestamp);
            preferences.putString("install_version", installVersion);

            installRND = MengineUtils.getSecureRandomNumber();

            preferences.putLong("install_rnd", installRND);
        }

        boolean mengine_session_use_install_id = this.getResourceBoolean(R.bool.mengine_session_use_install_id);

        if (userId == null && mengine_session_use_install_id == true) {
            userId = installId;

            preferences.putString("user_id", userId);
        }

        preferences.putLong("session_index", sessionIndex + 1);

        MenginePreferences.setPreferences(preferences);

        m_saveVersion = saveVersion;
        m_installId = installId;
        m_installTimestamp = installTimestamp;
        m_installVersion = installVersion;
        m_installRND = installRND;
        m_sessionIndex = sessionIndex;
        m_sessionTimestamp = sessionTimestamp;
        m_sessionRND = MengineUtils.getSecureRandomNumber();
        m_sessionId = MengineUtils.getSecureRandomHexString(32);

        m_userId = userId;

        m_adid = adid;
        m_acquisitionNetwork = acquisitionNetwork;
        m_acquisitionCampaign = acquisitionCampaign;

        MengineStatistic.load(this);

        this.setState("user.save_version", m_saveVersion);
        this.setState("user.install_id", m_installId);
        this.setState("user.install_timestamp", m_installTimestamp);
        this.setState("user.install_version", m_installVersion);
        this.setState("user.install_rnd", m_installRND);
        this.setState("user.session_index", m_sessionIndex);
        this.setState("user.session_timestamp", m_sessionTimestamp);
        this.setState("user.session_rnd", m_sessionRND);
        this.setState("user.session_id", m_sessionId);
        this.setState("user.acquisition_network", m_acquisitionNetwork);
        this.setState("user.acquisition_campaign", m_acquisitionCampaign);

        MengineAnalytics.addContextParameterBoolean("is_dev", BuildConfig.DEBUG);
        MengineAnalytics.addContextParameterString("install_id", m_installId);
        MengineAnalytics.addContextParameterLong("install_timestamp", m_installTimestamp);
        MengineAnalytics.addContextParameterString("install_version", m_installVersion);
        MengineAnalytics.addContextParameterLong("install_rnd", m_installRND);
        MengineAnalytics.addContextParameterLong("session_index", m_sessionIndex);
        MengineAnalytics.addContextParameterLong("session_timestamp", m_sessionTimestamp);
        MengineAnalytics.addContextParameterLong("session_rnd", m_sessionRND);
        MengineAnalytics.addContextParameterString("session_id", m_sessionId);
        MengineAnalytics.addContextGetterParameterString("adid", () -> {
            return m_adid;
        });
        MengineAnalytics.addContextGetterParameterString("acquisition_network", () -> {
            return m_acquisitionNetwork;
        });
        MengineAnalytics.addContextGetterParameterString("acquisition_campaign", () -> {
            return m_acquisitionCampaign;
        });

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

        if (this.getResourceBoolean(R.bool.mengine_secure_allow_android_id) == true) {
            m_androidId = this.getSecureAndroidId();
        } else {
            m_androidId = "0000000000000000";
        }

        ClassLoader cl = MengineApplication.class.getClassLoader();

        MengineNative.AndroidEnv_setMengineAndroidClassLoaderJNI(cl);

        String engine_gitsha1 = this.getEngineGITSHA1();
        this.setState("engine.gitsha1", engine_gitsha1);

        this.setState("application.init", "services_version");

        this.setState("application.init", "services_prepare");

        for (MengineListenerApplication l : applicationListeners) {
            try {
                long service_timestamp = MengineUtils.getTimestamp();

                l.onAppPrepare(this);

                MengineLog.logInfo(TAG, "onAppPrepare service: %s time: %d"
                    , l.getServiceName()
                    , MengineUtils.getTimestamp() - service_timestamp
                );
            } catch (final MengineServiceInvalidInitializeException e) {
                MengineAnalytics.buildEvent("mng_app_create_failed")
                    .addParameterException("exception", e)
                    .logAndFlush();

                this.invalidInitialize(e, Map.of("service", l.getServiceName()));

                return;
            }
        }

        this.setState("application.init", "services_load");

        this.onServicesLoad();

        this.setState("application.init", "services_create");

        for (MengineListenerApplication l : applicationListeners) {
            try {
                String serviceName = l.getServiceName();

                long service_timestamp = MengineUtils.getTimestamp();

                l.onAppCreate(this);

                MengineLog.logInfo(TAG, "onAppCreate service: %s time: %d"
                    , serviceName
                    , MengineUtils.getTimestamp() - service_timestamp
                );
            } catch (final MengineServiceInvalidInitializeException e) {
                MengineAnalytics.buildEvent("mng_app_create_failed")
                    .addParameterException("exception", e)
                    .logAndFlush();

                this.invalidInitialize(e, Map.of("service", l.getServiceName()));

                return;
            }
        }

        this.setState("application.init", "services_post");

        for (MengineListenerApplication l : applicationListeners) {
            try {
                String serviceName = l.getServiceName();

                long service_timestamp = MengineUtils.getTimestamp();

                l.onAppPost(this);

                MengineLog.logInfo(TAG, "onAppPost service: %s time: %d"
                    , serviceName
                    , MengineUtils.getTimestamp() - service_timestamp
                );
            } catch (final MengineServiceInvalidInitializeException e) {
                MengineAnalytics.buildEvent("mng_app_create_failed")
                    .addParameterException("exception", e)
                    .logAndFlush();

                this.invalidInitialize(e, Map.of("service", l.getServiceName()));

                return;
            }
        }

        Object nativeApplication = this.createNativeApplication();

        if (nativeApplication == null) {
            return;
        }

        m_nativeApplication = nativeApplication;

        for (MengineServiceInterface p : m_services) {
            if (p.onAvailable(this) == false) {
                continue;
            }

            if (p.isEmbedding() == false) {
                continue;
            }

            MengineTag tag = p.getServiceTag();

            MengineNative.AndroidKernelService_addPlugin(tag.toString(), p);
        }

        this.setState("application.init", "run_main");

        MengineNative.AndroidKernelService_addPlugin("Application", this);

        m_main = new MengineMain(m_nativeApplication);
        m_main.start();

        this.setState("application.init", "completed");

        MengineLog.logInfo(TAG, "[END] onCreate time: %d", MengineUtils.getTimestamp() - sessionTimestamp);
    }

    public void onServicesLoad() {
        MengineLog.logInfo(TAG, "[BEGIN] onServicesLoad");

        List<MengineServiceInterface> services = this.getServices();

        for (MengineServiceInterface s : services) {
            String serviceName = s.getServiceName();

            Bundle bundle = MenginePreferences.getPreferenceBundle("service." + serviceName, null);

            if (bundle == null) {
                continue;
            }

            MengineLog.logInfo(TAG, "onLoad service: %s bundle: %s"
                , serviceName
                , bundle
            );

            s.onLoad(this, bundle);
        }

        MengineLog.logInfo(TAG, "[END] onServicesLoad");
    }

    public void onServicesSave() {
        MengineLog.logInfo(TAG, "[BEGIN] onServicesSave");

        List<MengineServiceInterface> services = this.getServices();

        for (MengineServiceInterface s : services) {
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

        MengineLog.logInfo(TAG, "[END] onServicesSave");
    }

    public void forceSaveService(@NonNull MengineServiceInterface s, Bundle bundle) {
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

        MenginePreferences.setPreferenceBundle("service." + serviceName, bundle);
    }

    @Override
    public void onTerminate() {
        MengineLog.logInfo(TAG, "[BEGIN] onTerminate");

        this.onServicesSave();

        List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

        for (MengineListenerApplication l : applicationListeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onAppTerminate(this);
        }

        for (MengineServiceInterface p : m_services) {
            if (p.onAvailable(this) == false) {
                continue;
            }

            if (p.isEmbedding() == false) {
                continue;
            }

            MengineTag tag = p.getServiceTag();

            MengineNative.AndroidKernelService_removePlugin(tag.toString());
        }

        for (MengineServiceInterface service : m_services) {
            service.onAppFinalize(this);
        }

        MengineNative.AndroidKernelService_removePlugin("Application");

        MengineNative.AndroidPlatform_quitEvent();

        m_main.stop();
        m_main = null;

        this.destroyNativeApplication();

        MengineNative.AndroidEnv_removeMengineAndroidClassLoaderJNI();

        MengineLog.logInfo(TAG, "[END] onTerminate");

        super.onTerminate();
    }

    @Override
    public void attachBaseContext(Context base) {
        super.attachBaseContext(base);

        MengineLog.logInfo(TAG, "[BEGIN] attachBaseContext");

        List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

        for (MengineListenerApplication l : applicationListeners) {
            l.onAppAttachBaseContext(this, base);
        }

        MengineLog.logInfo(TAG, "[END] attachBaseContext");
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        MengineLog.logInfo(TAG, "[BEGIN] onConfigurationChanged config: %s"
            , newConfig.toString()
        );

        List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

        for (MengineListenerApplication l : applicationListeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onAppConfigurationChanged(this, newConfig);
        }

        MengineLog.logInfo(TAG, "[END] onConfigurationChanged");
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();

        MengineLog.logInfo(TAG, "[BEGIN] onLowMemory");

        List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

        for (MengineListenerApplication l : applicationListeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onAppLowMemory(this);
        }

        this.setState("activity.low_memory", true);

        MengineNative.AndroidPlatform_lowMemory();

        MengineLog.logInfo(TAG, "[END] onLowMemory");
    }

    @Override
    public void onTrimMemory(int level) {
        super.onTrimMemory(level);

        MengineLog.logInfo(TAG, "[BEGIN] onTrimMemory level: %d", level);

        List<MengineListenerApplication> applicationListeners = this.getApplicationListeners();

        for (MengineListenerApplication l : applicationListeners) {
            if (l.onAvailable(this) == false) {
                continue;
            }

            l.onAppTrimMemory(this, level);
        }

        this.setState("activity.trim_memory", level);

        MengineNative.AndroidPlatform_trimMemory(level);

        MengineLog.logInfo(TAG, "[END] onTrimMemory");
    }

    public void activateSemaphore(String name) {
        MengineLog.logInfo(TAG, "activateSemaphore semaphore: %s"
            , name
        );

        this.setState("native.semaphore", name);

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
        }

        MengineNative.AndroidKernelService_activateSemaphore(name);
    }

    public void deactivateSemaphore(String name) {
        MengineLog.logInfo(TAG, "deactivateSemaphore semaphore: %s"
            , name
        );

        synchronized (m_syncronizationSemaphores) {
            m_semaphores.remove(name);
        }
    }

    public boolean waitSemaphore(String name) {
        synchronized (m_syncronizationSemaphores) {
            MengineSemaphore semaphore = m_semaphores.get(name);

            if (semaphore == null) {
                semaphore = new MengineSemaphore(false);

                m_semaphores.put(name, semaphore);

                return false;
            }

            if (semaphore.isActivated() == false) {
                return false;
            }
        }

        return true;
    }

    public void nativeCall(@NonNull String plugin, String method, Object ... args) {
        if (BuildConfig.DEBUG == true) {
            MengineLog.logInfo(TAG, "nativeCall plugin [%s] method [%s] args [%s]"
                , plugin
                , method
                , Arrays.toString(args)
            );
        }

        this.setState("native.call", plugin + "." + method);

        MengineNative.AndroidKernelService_call(plugin, method, args);
    }

    private void invalidInitialize(@NonNull MengineServiceInvalidInitializeException e, @NonNull Map<String, Object> attributes) {
        MengineLog.logException(TAG, e, attributes);

        m_invalidInitialize = true;
        m_invalidInitializeException = e;
    }
}