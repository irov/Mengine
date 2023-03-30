package org.Mengine.Plugin.Sentry;

import android.os.Bundle;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MenginePluginLoggerListener;

import io.sentry.Sentry;
import io.sentry.SentryLevel;
import io.sentry.android.core.SentryAndroid;

public class MengineSentryPlugin extends MenginePlugin implements MenginePluginLoggerListener {
    public static final String PLUGIN_NAME = "Sentry";

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        String MengineSentryPlugin_DSN = activity.getMetaDataString("mengine.sentry.dsn");

        if (MengineSentryPlugin_DSN == null) {
            this.invalidInitialize("invalid setup meta data [mengine.sentry.dsn]");

            return;
        }

        MengineSentryPlugin.this.logInfo("Sentry DNS: %s"
            , MengineSentryPlugin_DSN
        );

        SentryAndroid.init(activity, options -> {
            options.setDsn(MengineSentryPlugin_DSN);

            String buildVersion = activity.getBuildVersion();
            options.setRelease(buildVersion);

            options.setAttachStacktrace(false);
            options.setAttachThreads(false);
        });
    }

    @Override
    public void onMengineCreateApplication(MengineActivity activity) {
        Sentry.configureScope(scope -> {
            String companyName = activity.getCompanyName();
            String projectName = activity.getProjectName();
            int projectVersion = activity.getProjectVersion();
            boolean isDebugMode = activity.isDebugMode();
            boolean isDevelopmentMode = activity.isDevelopmentMode();
            boolean isMasterRelease = activity.isMasterRelease();
            boolean isBuildPublish = activity.isBuildPublish();
            String engineGITSHA1 = activity.getEngineGITSHA1();
            String buildTimestamp = activity.getBuildTimestamp();
            String buildUsername = activity.getBuildUsername();

            scope.setExtra("Company", companyName);
            scope.setExtra("Project", projectName);
            scope.setExtra("Version", String.valueOf(projectVersion));
            scope.setExtra("Debug", String.valueOf(isDebugMode));
            scope.setExtra("Development", String.valueOf(isDevelopmentMode));
            scope.setExtra("Master", String.valueOf(isMasterRelease));
            scope.setExtra("Publish", String.valueOf(isBuildPublish));
            scope.setExtra("Engine Commit", engineGITSHA1);
            scope.setExtra("Build Timestamp", buildTimestamp);
            scope.setExtra("Build Username", buildUsername);
        });
    }

    @Override
    public void onMengineLogger(MengineApplication application, String category, int level, int filter, int color, String msg) {
        if (level > MengineLog.LM_ERROR) {
            return;
        }

        Sentry.captureMessage("[" + category + "] " + msg, SentryLevel.ERROR);
    }
}
