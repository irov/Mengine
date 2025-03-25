package org.Mengine.Plugin.DataDog;

import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;

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
import org.Mengine.Base.MengineListenerSessionId;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Map;

public class MengineDataDogPlugin extends MengineService implements MengineListenerLogger, MengineListenerApplication, MengineListenerActivity, MengineListenerSessionId {
    public static final String SERVICE_NAME = "DataDog";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final String METADATA_SITE = "mengine.datadog.site";
    public static final String METADATA_CLIENT_TOKEN = "mengine.datadog.client_token";

    private Logger m_loggerDataDog;

    @Override
    public void onAppPrepare(@NonNull MengineApplication application, @NonNull Map<String, String> pluginVersions) throws MengineServiceInvalidInitializeException {
        String MengineDataDogPlugin_Site = this.getMetaDataString(METADATA_SITE);

        this.logInfo("%s: %s"
            , METADATA_SITE
            , MengineUtils.getRedactedValue(MengineDataDogPlugin_Site)
        );

        DatadogSite site;

        if (MengineDataDogPlugin_Site.equals("us1") == true) {
            site = DatadogSite.US1;
        } else if (MengineDataDogPlugin_Site.equals("us3") == true) {
            site = DatadogSite.US3;
        } else if (MengineDataDogPlugin_Site.equals("us5") == true) {
            site = DatadogSite.US5;
        } else if (MengineDataDogPlugin_Site.equals("us1_fed") == true) {
            site = DatadogSite.US1_FED;
        } else if (MengineDataDogPlugin_Site.equals("eu1") == true) {
            site = DatadogSite.EU1;
        } else {
            this.invalidInitialize("initialize unknown site: %s"
                , MengineDataDogPlugin_Site
            );

            return;
        }

        String MengineDataDogPlugin_ClientToken = this.getMetaDataString(METADATA_CLIENT_TOKEN);

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

        if (Datadog.initialize(context, config, TrackingConsent.GRANTED) == null) {
            this.invalidInitialize("initialize failed");

            return;
        }

        if (BuildConfig.DEBUG == true) {
            Datadog.setVerbosity(Log.INFO);
        }

        String installKey = application.getInstallKey();
        String sessionId = application.getSessionId();

        Datadog.setUserInfo(sessionId, null, null, Map.of("install_key", installKey));

        LogsConfiguration logsConfig = new LogsConfiguration.Builder()
            .build();
        Logs.enable(logsConfig);

        Logger loggerDataDog = new Logger.Builder()
            .setNetworkInfoEnabled(false)
            .setLogcatLogsEnabled(false)
            .build();

        try {
            JSONObject version_attribute = new JSONObject();

            int versionCode = application.getVersionCode();
            version_attribute.put("code", versionCode);

            String versionName = application.getVersionName();
            version_attribute.put("name", versionName);

            loggerDataDog.addAttribute("version", version_attribute);

            JSONObject install_attribute = new JSONObject();

            install_attribute.put("key", installKey);

            long installTimestamp = application.getInstallTimestamp();
            install_attribute.put("timestamp", installTimestamp);

            String installVersion = application.getInstallVersion();
            install_attribute.put("version", installVersion);

            long installRND = application.getInstallRND();
            install_attribute.put("rnd", installRND);

            loggerDataDog.addAttribute("install", install_attribute);

            JSONObject session_attribute = new JSONObject();

            long sessionIndex = application.getSessionIndex();
            session_attribute.put("index", sessionIndex);

            long sessionTimestamp = application.getSessionTimestamp();
            session_attribute.put("timestamp", sessionTimestamp);

            loggerDataDog.addAttribute("session", session_attribute);
        } catch (JSONException e) {
            this.logError("initialize attribute exception: %s"
                , e.getMessage()
            );
        }

        m_loggerDataDog = loggerDataDog;
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        m_loggerDataDog = null;
    }

    @Override
    public void onMengineSetSessionId(@NonNull MengineApplication application, String sessionId) {
        String installKey = application.getInstallKey();

        Datadog.setUserInfo(sessionId, null, null, Map.of("install_key", installKey));
    }

    @Override
    public void onMengineRemoveSessionData(@NonNull MengineApplication application) {
        Datadog.setUserInfo(null, null, null, null);
    }

    @Override
    public void onMengineLogger(@NonNull MengineApplication application, int level, int filter, String tag, String msg) {
        if (m_loggerDataDog == null) {
            return;
        }

        switch (level) {
            case MengineLog.LM_VERBOSE:
                return;
            case MengineLog.LM_DEBUG:
                return;
            case MengineLog.LM_INFO:
            case MengineLog.LM_MESSAGE:
            case MengineLog.LM_MESSAGE_RELEASE:
                if (BuildConfig.DEBUG == true) {
                    m_loggerDataDog.i(msg, null, Map.of("tag", tag));
                }
                break;
            case MengineLog.LM_WARNING:
                m_loggerDataDog.w(msg, null, Map.of("tag", tag));
                break;
            case MengineLog.LM_ERROR:
                m_loggerDataDog.e(msg, null, Map.of("tag", tag));
                break;
            case MengineLog.LM_FATAL:
                m_loggerDataDog.wtf(msg, null, Map.of("tag", tag));
                break;
        }
    }
}
