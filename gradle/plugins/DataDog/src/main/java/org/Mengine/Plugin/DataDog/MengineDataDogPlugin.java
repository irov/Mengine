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
import org.Mengine.Base.MenginePluginLoggerListener;

import java.util.Map;

public class MengineDataDogPlugin extends MenginePlugin implements MenginePluginLoggerListener {
    public static final String PLUGIN_NAME = "DevToDev";
    public static final boolean PLUGIN_EMBEDDING = true;

    private Logger m_loggerDataDog;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        Configuration config = new Configuration.Builder(
            true,
            true,
            true,
            false)
            .useSite(DatadogSite.EU1)
            .build();

        Credentials credentials = new Credentials(
            "pubda43545394399088f99470a48958f89c",
            "production",
            Credentials.NO_VARIANT,
            "",
            null
        );

        Datadog.initialize(activity, credentials, config, TrackingConsent.GRANTED);

        Datadog.setVerbosity(Log.VERBOSE);

        Logger loggerDataDog = new Logger.Builder()
            .setNetworkInfoEnabled(true)
            .setLogcatLogsEnabled(false)
            .setDatadogLogsEnabled(true)
            .setLoggerName("MengineDataDog")
            .build();

        MengineApplication application = activity.getMengineApplication();

        String installKey = application.getInstallKey();
        loggerDataDog.addAttribute("install_key", installKey);

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
