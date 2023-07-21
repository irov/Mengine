package org.Mengine.Plugin.DataDog;

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
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MenginePluginLoggerListener;

import java.util.Map;

public class MengineDataDogPlugin extends MenginePlugin implements MenginePluginLoggerListener {
    public static final String PLUGIN_NAME = "DevToDev";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_SITE = "mengine.datadog.site";
    public static final String PLUGIN_METADATA_CLIENT_TOKEN = "mengine.datadog.client_token";
    public static final String PLUGIN_METADATA_SERVICE_NAME = "mengine.datadog.service_name";

    private Logger m_loggerDataDog;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        String MengineDataDogPlugin_Site = activity.getMetaDataString(PLUGIN_METADATA_SITE);

        if (MengineDataDogPlugin_Site == null) {
            this.invalidInitialize("initialize unsetup [%s]", PLUGIN_METADATA_SITE);

            return;
        }

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

        String MengineDataDogPlugin_ClientToken = activity.getMetaDataString(PLUGIN_METADATA_CLIENT_TOKEN);

        if (MengineDataDogPlugin_ClientToken == null) {
            this.invalidInitialize("initialize unsetup [%s]", PLUGIN_METADATA_CLIENT_TOKEN);

            return;
        }

        boolean logsEnabled = true;
        boolean tracesEnabled = true;
        boolean crashReportsEnabled = true;
        boolean rumEnabled = false;

        Configuration config = new Configuration.Builder(logsEnabled, tracesEnabled, crashReportsEnabled, rumEnabled)
            .useSite(site)
            .build();

        String MengineDataDogPlugin_ServiceName = activity.getMetaDataString(PLUGIN_METADATA_SERVICE_NAME);

        String clientToken = MengineDataDogPlugin_ClientToken;
        String envName = "production";
        String variant = Credentials.NO_VARIANT;
        String rumApplicationId = "";
        String serviceName = MengineDataDogPlugin_ServiceName;

        Credentials credentials = new Credentials(clientToken, envName, variant, rumApplicationId, serviceName);

        Datadog.initialize(activity, credentials, config, TrackingConsent.GRANTED);

        if (BuildConfig.DEBUG == true) {
            Datadog.setVerbosity(Log.VERBOSE);
        }

        Logger loggerDataDog = new Logger.Builder()
            .setNetworkInfoEnabled(true)
            .setLogcatLogsEnabled(false)
            .setDatadogLogsEnabled(true)
            .setLoggerName("MengineDataDog")
            .build();

        MengineApplication application = activity.getMengineApplication();

        String installKey = application.getInstallKey();
        loggerDataDog.addAttribute("install_key", installKey);

        String sessionId = application.getSessionId();
        loggerDataDog.addAttribute("session_id", sessionId);

        long sessionIndex = application.getSessionIndex();
        loggerDataDog.addAttribute("session_index", sessionIndex);

        m_loggerDataDog = loggerDataDog;
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        m_loggerDataDog = null;
    }

    @Override
    public void onMengineLogger(MengineApplication application, String tag, int level, int filter, int color, String msg) {
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
