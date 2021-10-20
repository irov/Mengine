package org.Mengine.Build.Sentry;

import android.os.Bundle;
import android.util.Log;

import org.Mengine.Build.MenginePlugin;
import org.Mengine.Build.MengineActivity;

import io.sentry.Sentry;
import io.sentry.android.core.SentryAndroid;

public class MengineSentryPlugin extends MenginePlugin {
    private static final String TAG = "Sentry";
    private MengineActivity m_activity;

    public MengineSentryPlugin(MengineActivity _activity) {
        m_activity = _activity;
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
    }
    @Override
    public void onMengineInitializeBaseServices()
    {
        SentryAndroid.init(m_activity, options -> {
            String SENTRY_DNS = m_activity.getConfigValue("Sentry", "DSN", "");
            options.setDsn(SENTRY_DNS);
            Log.i(TAG, "Sentry DNS:" + SENTRY_DNS);

            String buildVersion = m_activity.getBuildVersion();
            options.setRelease(buildVersion);
            Log.i(TAG, "Sentry Build Version:" + buildVersion);
        });
    }
    @Override
    public void onMengineCreateApplication()
    {
        Sentry.configureScope(scope -> {
            String SENTRY_APPLICATION = m_activity.getConfigValue( "Sentry", "Application", "Mengine" );

            String companyName = m_activity.getCompanyName();
            String projectName = m_activity.getProjectName();
            int projectVersion = m_activity.getProjectVersion();
            boolean isDebugMode = m_activity.isDebugMode();
            boolean isDevelopmentMode = m_activity.isDevelopmentMode();
            boolean isBuildMaster = m_activity.isBuildMaster();
            boolean isBuildPublish = m_activity.isBuildPublish();
            String engineGITSHA1 = m_activity.getEngineGITSHA1();
            String buildTimestamp = m_activity.getBuildTimestamp();
            String buildUsername = m_activity.getBuildUsername();

            scope.setExtra("Application", SENTRY_APPLICATION);
            scope.setExtra("Company", companyName);
            scope.setExtra("Project", projectName);
            scope.setExtra("Version", String.valueOf(projectVersion));
            scope.setExtra("Debug", String.valueOf(isDebugMode));
            scope.setExtra("Development", String.valueOf(isDevelopmentMode));
            scope.setExtra("Master", String.valueOf(isBuildMaster));
            scope.setExtra("Publish", String.valueOf(isBuildPublish));
            scope.setExtra("Engine Commit", engineGITSHA1);
            scope.setExtra("Build Timestamp", buildTimestamp);
            scope.setExtra("Build Username", buildUsername);
        });
    }
}
