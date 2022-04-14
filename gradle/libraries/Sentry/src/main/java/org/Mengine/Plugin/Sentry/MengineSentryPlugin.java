package org.Mengine.Plugin.Sentry;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import io.sentry.Sentry;
import io.sentry.android.core.SentryAndroid;

public class MengineSentryPlugin extends MenginePlugin {
    @Override
    public void onMengineInitializeBaseServices(MengineActivity activity)
    {
        SentryAndroid.init(activity, options -> {
            String SENTRY_DNS = activity.getConfigValue("Sentry", "DSN", "");
            options.setDsn(SENTRY_DNS);
            this.log("Sentry DNS: %s", SENTRY_DNS);

            String buildVersion = activity.getBuildVersion();
            options.setRelease(buildVersion);
        });
    }

    @Override
    public void onMengineCreateApplication(MengineActivity activity)
    {
        Sentry.configureScope(scope -> {
            String SENTRY_APPLICATION = activity.getConfigValue( "Sentry", "Application", "Mengine" );

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

            scope.setExtra("Application", SENTRY_APPLICATION);
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
}
