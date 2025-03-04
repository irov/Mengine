package org.Mengine.Plugin.DataDog;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import com.datadog.android.Datadog;
import com.datadog.android.DatadogSite;
import com.datadog.android.core.configuration.Configuration;
import com.datadog.android.core.configuration.Credentials;
import com.datadog.android.log.Logger;
import com.datadog.android.privacy.TrackingConsent;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MenginePluginLoggerListener;

import java.util.Map;

public class MengineDataDogPlugin extends MenginePlugin implements MenginePluginLoggerListener, MenginePluginApplicationListener, MenginePluginActivityListener, MenginePluginSessionIdListener {
    public static final String SERVICE_NAME = "DataDog";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final String METADATA_SITE = "mengine.datadog.site";
    public static final String METADATA_CLIENT_TOKEN = "mengine.datadog.client_token";
    public static final String METADATA_SERVICE_NAME = "mengine.datadog.service_name";

    private Logger m_loggerDataDog;

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String MengineDataDogPlugin_Site = this.getMetaDataString(METADATA_SITE);

        this.logInfo("%s: %s"
            , METADATA_SITE
            , MengineDataDogPlugin_Site
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
            , MengineDataDogPlugin_ClientToken
        );

        boolean logsEnabled = true;
        boolean tracesEnabled = true;
        boolean crashReportsEnabled = true;
        boolean rumEnabled = false;

        Configuration config = new Configuration.Builder(logsEnabled, tracesEnabled, crashReportsEnabled, rumEnabled)
            .useSite(site)
            .build();

        String MengineDataDogPlugin_ServiceName = this.getMetaDataString(METADATA_SERVICE_NAME);

        this.logInfo("%s: %s"
            , METADATA_SERVICE_NAME
            , MengineDataDogPlugin_ServiceName
        );

        String clientToken = MengineDataDogPlugin_ClientToken;
        String envName = "production";
        String variant = Credentials.NO_VARIANT;
        String rumApplicationId = "";
        String serviceName = MengineDataDogPlugin_ServiceName;

        Credentials credentials = new Credentials(clientToken, envName, variant, rumApplicationId, serviceName);

        Context context = application.getApplicationContext();

        Datadog.initialize(context, credentials, config, TrackingConsent.GRANTED);

        if (BuildConfig.DEBUG == true) {
            Datadog.setVerbosity(Log.VERBOSE);
        }

        Logger loggerDataDog = new Logger.Builder()
            .setNetworkInfoEnabled(true)
            .setLogcatLogsEnabled(false)
            .setDatadogLogsEnabled(true)
            .setLoggerName("MengineDataDog")
            .build();

        String installKey = application.getInstallKey();
        loggerDataDog.addAttribute("install_key", installKey);

        long installTimestamp = application.getInstallTimestamp();
        loggerDataDog.addAttribute("install_timestamp", installTimestamp);

        String installVersion = application.getInstallVersion();
        loggerDataDog.addAttribute("install_version", installVersion);

        long installRND = application.getInstallRND();
        loggerDataDog.addAttribute("install_rnd", installRND);

        long sessionIndex = application.getSessionIndex();
        loggerDataDog.addAttribute("session_index", sessionIndex);

        long sessionTimestamp = application.getSessionTimestamp();
        loggerDataDog.addAttribute("session_timestamp", sessionTimestamp);

        String sessionId = application.getSessionId();
        loggerDataDog.addAttribute("session_id", sessionId);

        m_loggerDataDog = loggerDataDog;
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        m_loggerDataDog = null;
    }

    @Override
    void onMengineSessionId(MengineApplication application, String sessionId) {
        if (m_loggerDataDog == null) {
            return;
        }

        m_loggerDataDog.addAttribute("session_id", sessionId);
    }

    @Override
    public void onMengineLogger(MengineApplication application, int level, int filter, String tag, String msg) {
        if (m_loggerDataDog == null) {
            return;
        }

        switch (level) {
            case MengineLog.LM_VERBOSE:
                m_loggerDataDog.v(msg, null, Map.of("tag", tag));
                break;
            case MengineLog.LM_DEBUG:
                m_loggerDataDog.d(msg, null, Map.of("tag", tag));
                break;
            case MengineLog.LM_INFO:
            case MengineLog.LM_MESSAGE:
            case MengineLog.LM_MESSAGE_RELEASE:
                m_loggerDataDog.i(msg, null, Map.of("tag", tag));
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
