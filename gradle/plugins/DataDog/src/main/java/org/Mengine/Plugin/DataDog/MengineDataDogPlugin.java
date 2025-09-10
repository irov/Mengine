package org.Mengine.Plugin.DataDog;

import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

import com.datadog.android.Datadog;
import com.datadog.android.DatadogSite;
import com.datadog.android.core.configuration.Configuration;
import com.datadog.android.log.Logger;
import com.datadog.android.log.Logs;
import com.datadog.android.log.LogsConfiguration;
import com.datadog.android.privacy.TrackingConsent;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerLogger;
import org.Mengine.Base.MengineListenerRemoteConfig;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineParamLoggerException;
import org.Mengine.Base.MengineParamLoggerMessage;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineUtils;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Map;
import java.util.Objects;

public class MengineDataDogPlugin extends MengineService implements MengineListenerLogger, MengineListenerApplication, MengineListenerActivity, MengineListenerUser, MengineListenerRemoteConfig, MengineListenerTransparencyConsent {
    public static final String SERVICE_NAME = "DataDog";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final @StringRes int METADATA_SITE = R.string.mengine_datadog_site;
    public static final @StringRes int METADATA_CLIENT_TOKEN = R.string.mengine_datadog_client_token;

    private volatile boolean m_enableDebugMessage = false;
    private volatile boolean m_enableInfoMessage = false;

    private Logger m_loggerDataDog;

    protected boolean getEnableDebugMessage() {
        return m_enableDebugMessage;
    }

    protected boolean getEnableInfoMessage() {
        return m_enableInfoMessage;
    }

    protected TrackingConsent getTrackingConsent(@NonNull MengineParamTransparencyConsent consentParam) {
        if (consentParam == null) {
            return TrackingConsent.GRANTED;
        }

        if (consentParam.isPending() == true) {
            return TrackingConsent.PENDING;
        }

        if (consentParam.getConsentAdStorage() == true) {
            return TrackingConsent.GRANTED;
        }

        return TrackingConsent.NOT_GRANTED;
    }

    @Override
    public void onAppPrepare(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        String MengineDataDogPlugin_Site = this.getResourceString(METADATA_SITE);

        this.logInfo("%s: %s"
            , METADATA_SITE
            , MengineUtils.getRedactedValue(MengineDataDogPlugin_Site)
        );

        DatadogSite site;

        if (Objects.equals(MengineDataDogPlugin_Site, "us1") == true) {
            site = DatadogSite.US1;
        } else if (Objects.equals(MengineDataDogPlugin_Site, "us3") == true) {
            site = DatadogSite.US3;
        } else if (Objects.equals(MengineDataDogPlugin_Site, "us5") == true) {
            site = DatadogSite.US5;
        } else if (Objects.equals(MengineDataDogPlugin_Site, "us1_fed") == true) {
            site = DatadogSite.US1_FED;
        } else if (Objects.equals(MengineDataDogPlugin_Site, "eu1") == true) {
            site = DatadogSite.EU1;
        } else {
            this.invalidInitialize("initialize unknown site: %s"
                , MengineDataDogPlugin_Site
            );

            return;
        }

        String MengineDataDogPlugin_ClientToken = this.getResourceString(METADATA_CLIENT_TOKEN);

        this.logInfo("%s: %s"
            , METADATA_CLIENT_TOKEN
            , MengineUtils.getRedactedValue(MengineDataDogPlugin_ClientToken)
        );

        boolean crashReportsEnabled = false;

        String clientToken = MengineDataDogPlugin_ClientToken;
        String envName = (BuildConfig.DEBUG == true) ? "dev" : "prod";
        String variant = "";

        Configuration config = new Configuration.Builder(clientToken, envName, variant)
            .useSite(site)
            .setCrashReportsEnabled(crashReportsEnabled)
            .build();

        Context context = application.getApplicationContext();

        MengineParamTransparencyConsent consentParam = application.makeTransparencyConsentParam();

        TrackingConsent consent = this.getTrackingConsent(consentParam);

        if (Datadog.initialize(context, config, consent) == null) {
            this.logError("Datadog initialize failed");

            return;
        }

        if (BuildConfig.DEBUG == true) {
            Datadog.setVerbosity(Log.INFO);
        }

        String installId = application.getInstallId();

        String userId = application.getUserId();
        if (userId != null && consent == TrackingConsent.GRANTED) {
            Datadog.setUserInfo(userId, null, null, Map.of("install_id", installId));
        }

        LogsConfiguration logsConfig = new LogsConfiguration.Builder()
            .build();
        Logs.enable(logsConfig);

        Logger logger = new Logger.Builder()
            .setNetworkInfoEnabled(false)
            .setLogcatLogsEnabled(false)
            .build();

        try {
            JSONObject attributes = new JSONObject();

            int versionCode = application.getVersionCode();
            attributes.put("version.code", versionCode);

            String versionName = application.getVersionName();
            attributes.put("version.name", versionName);

            attributes.put("install.id", installId);

            long installTimestamp = application.getInstallTimestamp();
            attributes.put("install.timestamp", installTimestamp);

            String installVersion = application.getInstallVersion();
            attributes.put("install.version", installVersion);

            long installRND = application.getInstallRND();
            attributes.put("install.rnd", installRND);

            String sessionId = application.getSessionId();
            attributes.put("session.id", sessionId);

            long sessionIndex = application.getSessionIndex();
            attributes.put("session.index", sessionIndex);

            long sessionTimestamp = application.getSessionTimestamp();
            attributes.put("session.timestamp", sessionTimestamp);

            long sessionRND = application.getSessionRND();
            attributes.put("session.rnd", sessionRND);

            String osVersion = application.getOSVersion();
            attributes.put("os.version", osVersion);

            int sdkVersion = application.getSDKVersion();
            attributes.put("os.sdk", sdkVersion);

            String deviceManufacturer = application.getDeviceManufacturer();
            attributes.put("device.manufacturer", deviceManufacturer);

            String deviceBrand = application.getDeviceBrand();
            attributes.put("device.brand", deviceBrand);

            String deviceModel = application.getDeviceModel();
            attributes.put("device.model", deviceModel);

            logger.addAttribute("mng_base", attributes);
        } catch (JSONException e) {
            this.logException(e, Map.of());
        }

        m_loggerDataDog = logger;
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        m_loggerDataDog = null;
    }

    @Override
    public void onMengineChangeUserId(@NonNull MengineApplication application, String oldUserId, String newUserId) {
        MengineParamTransparencyConsent consentParam = application.makeTransparencyConsentParam();

        TrackingConsent consent = this.getTrackingConsent(consentParam);

        if (consent != TrackingConsent.GRANTED) {
            return;
        }

        String installId = application.getInstallId();

        Datadog.setUserInfo(newUserId, null, null, Map.of("install_id", installId));
    }

    @Override
    public void onMengineRemoveUserData(@NonNull MengineApplication application) {
        Datadog.clearAllData();
    }

    @Override
    public void onMengineLog(@NonNull MengineApplication application, @NonNull MengineParamLoggerMessage message) {
        if (m_loggerDataDog == null) {
            return;
        }

        Map<String, Object> attributes = BuildConfig.DEBUG == true ? Map.of("mng_record", Map.of(
            "code.category", message.MESSAGE_CATEGORY,
            "code.thread", message.MESSAGE_THREAD,
            "code.file", message.MESSAGE_FILE != null ? message.MESSAGE_FILE : "empty",
            "code.line", message.MESSAGE_LINE,
            "code.function", message.MESSAGE_FUNCTION != null ? message.MESSAGE_FUNCTION : "empty"
            )
        ) : Map.of("mng_record", Map.of(
            "code.category", message.MESSAGE_CATEGORY,
            "code.thread", message.MESSAGE_THREAD
            )
        );

        switch (message.MESSAGE_LEVEL) {
            case MengineLog.LM_VERBOSE:
            case MengineLog.LM_DEBUG:
                return;
            case MengineLog.LM_INFO:
                boolean enableInfoMessage = this.getEnableInfoMessage();

                if (enableInfoMessage == true) {
                    m_loggerDataDog.i(message.MESSAGE_DATA, null, attributes);
                }
                return;
            case MengineLog.LM_MESSAGE:
            case MengineLog.LM_MESSAGE_RELEASE:
                boolean enableDebugMessage = this.getEnableDebugMessage();

                if (enableDebugMessage == true || BuildConfig.DEBUG == true) {
                    m_loggerDataDog.i(message.MESSAGE_DATA, null, attributes);
                }
                break;
            case MengineLog.LM_WARNING:
                m_loggerDataDog.w(message.MESSAGE_DATA, null, attributes);
                break;
            case MengineLog.LM_ERROR:
                m_loggerDataDog.e(message.MESSAGE_DATA, null, attributes);
                break;
            case MengineLog.LM_FATAL:
                m_loggerDataDog.wtf(message.MESSAGE_DATA, null, attributes);
                break;
        }
    }

    @Override
    public void onMengineException(@NonNull MengineApplication application, @NonNull MengineParamLoggerException exception) {
        if (m_loggerDataDog == null) {
            return;
        }

        m_loggerDataDog.wtf("MengineException", exception.EXCEPTION_THROWABLE, exception.EXCEPTION_ATTRIBUTES);
    }

    @Override
    public void onMengineRemoteConfigFetch(@NonNull MengineApplication application, boolean updated, @NonNull Map<String, JSONObject> configs) {
        JSONObject datadog_debug_message = configs.getOrDefault("datadog_debug_message", null);

        if (datadog_debug_message != null) {
            boolean enable = datadog_debug_message.optBoolean("enable", false);

            m_enableDebugMessage = enable;
        }

        JSONObject datadog_info_message = configs.getOrDefault("datadog_info_message", null);

        if (datadog_info_message != null) {
            boolean enable = datadog_info_message.optBoolean("enable", false);

            m_enableInfoMessage = enable;
        }
    }

    @Override
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, @NonNull MengineParamTransparencyConsent tcParam) {
        TrackingConsent consent = this.getTrackingConsent(tcParam);

        Datadog.setTrackingConsent(consent);
    }
}
