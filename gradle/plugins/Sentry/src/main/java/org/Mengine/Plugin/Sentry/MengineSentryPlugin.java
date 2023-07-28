package org.Mengine.Plugin.Sentry;

import android.content.Context;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MenginePluginLoggerListener;

import io.sentry.Sentry;
import io.sentry.SentryLevel;
import io.sentry.android.core.SentryAndroid;
import io.sentry.protocol.User;

public class MengineSentryPlugin extends MenginePlugin implements MenginePluginLoggerListener, MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "Sentry";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_DSN = "mengine.sentry.dsn";
    public static final String PLUGIN_METADATA_ENABLE_UNCAUGHT_EXCEPTION_HANDLER = "mengine.sentry.enable_uncaught_exception_handler";

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String MengineSentryPlugin_DSN = application.getMetaDataString(PLUGIN_METADATA_DSN);

        if (MengineSentryPlugin_DSN == null) {
            this.invalidInitialize("invalid setup meta data [%s]", PLUGIN_METADATA_DSN);

            return;
        }

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_DSN
            , MengineSentryPlugin_DSN
        );

        boolean MengineSentryPlugin_EnableUncaughtExceptionHandler = application.getMetaDataBoolean(PLUGIN_METADATA_ENABLE_UNCAUGHT_EXCEPTION_HANDLER, true);

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_ENABLE_UNCAUGHT_EXCEPTION_HANDLER
            , MengineSentryPlugin_DSN
        );

        Context context = application.getApplicationContext();

        SentryAndroid.init(context, options -> {
            options.setDsn(MengineSentryPlugin_DSN);

            if (BuildConfig.DEBUG == true) {
                options.setEnvironment("debug");
            } else {
                if (application.isBuildPublish() == false) {
                    options.setEnvironment("dev");
                } else {
                    options.setEnvironment("publish");
                }
            }

            String versionName = application.getVersionName();
            options.setRelease(versionName);

            String appplicationId = application.getApplicationId();
            options.setTag("app", appplicationId);

            options.setAttachStacktrace(false);
            options.setAttachThreads(false);
            options.setEnableAutoSessionTracking(false);

            options.setEnableUncaughtExceptionHandler(MengineSentryPlugin_EnableUncaughtExceptionHandler);
        });
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        //Empty
    }

    @Override
    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_SESSION_ID) {
            String sessionId = (String)args[0];

            User user = new User();
            user.setId(sessionId);

            Sentry.setUser(user);
        }
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
        if (BuildConfig.DEBUG == true) {
            return;
        }

        switch (level) {
            case MengineLog.LM_ERROR:
                Sentry.captureMessage("[" + category + "] " + msg, SentryLevel.ERROR);
                break;
            case MengineLog.LM_FATAL:
                Sentry.captureMessage("[" + category + "] " + msg, SentryLevel.FATAL);
                break;
        }
    }

    @Override
    public void onMengineCaughtException(MengineApplication activity, Throwable throwable) {
        this.recordException(throwable);
    }

    @Override
    public void onState(MengineApplication application, String name, Object value) {
        this.setCustomKey("." + name, value);
    }

    public void setCustomKey(String key, Object value) {
        this.logMessage("setCustomKey key: %s value: %s"
            , key
            , value
        );

        Sentry.setExtra(key, String.valueOf(value));
    }

    public void recordException(Throwable throwable) {
        this.logMessage("recordException throwable: %s"
            , throwable.getLocalizedMessage()
        );

        throwable.printStackTrace(System.err);

        Sentry.captureException(throwable);
    }

    public void testCrash() {
        this.logMessage("testCrash");

        this.setCustomKey("test.crash", true);

        throw new RuntimeException("Sentry Test Crash");
    }
}
