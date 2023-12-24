package org.Mengine.Plugin.Sentry;

import android.content.Context;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginEngineListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import io.sentry.Sentry;
import io.sentry.android.core.SentryAndroid;
import io.sentry.protocol.User;

public class MengineSentryPlugin extends MenginePlugin implements MenginePluginApplicationListener, MenginePluginEngineListener {
    public static final String PLUGIN_NAME = "Sentry";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_DSN = "mengine.sentry.dsn";
    public static final String PLUGIN_METADATA_ENABLE_UNCAUGHT_EXCEPTION_HANDLER = "mengine.sentry.enable_uncaught_exception_handler";

    public static boolean m_passGDPR = false;

    @Override
    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_GDPR_PASS) {
            boolean passGDPR = (boolean)args[0];

            m_passGDPR = passGDPR;
        } else if (event == MengineEvent.EVENT_SESSION_ID) {
            String sessionId = (String)args[0];

            User user = new User();
            user.setId(sessionId);

            Sentry.setUser(user);
        }
    }

    @Override
    public void onAppInit(MengineApplication application, boolean isMainProcess) throws MenginePluginInvalidInitializeException {
        if (isMainProcess == false) {
            return;
        }

        String MengineSentryPlugin_DSN = this.getMetaDataString(PLUGIN_METADATA_DSN);

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_DSN
            , MengineSentryPlugin_DSN
        );

        boolean MengineSentryPlugin_EnableUncaughtExceptionHandler = this.getMetaDataBoolean(PLUGIN_METADATA_ENABLE_UNCAUGHT_EXCEPTION_HANDLER);

        this.logInfo("%s: %b"
            , PLUGIN_METADATA_ENABLE_UNCAUGHT_EXCEPTION_HANDLER
            , MengineSentryPlugin_EnableUncaughtExceptionHandler
        );

        Context context = application.getApplicationContext();

        SentryAndroid.init(context, options -> {
            options.setDsn(MengineSentryPlugin_DSN);

            if (application.isBuildPublish() == true) {
                options.setEnvironment("production");
            } else {
                options.setEnvironment("debug");
            }

            String versionName = application.getVersionName();
            options.setRelease(versionName);

            String appplicationId = application.getApplicationId();
            options.setTag("app", appplicationId);

            options.setAttachStacktrace(false);
            options.setAttachThreads(false);

            options.setEnableUncaughtExceptionHandler(MengineSentryPlugin_EnableUncaughtExceptionHandler);

            options.setBeforeSend((event, hint) -> {
                if (m_passGDPR == false) {
                    return null;
                }

                return event;
            });

            options.setBeforeSendTransaction((transaction, hint) -> {
                if (m_passGDPR == false) {
                    return null;
                }

                return transaction;
            });
        });
    }

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        Sentry.configureScope(scope -> {
            boolean isMasterRelease = application.isMasterRelease();
            boolean isBuildPublish = application.isBuildPublish();
            String engineGITSHA1 = application.getEngineGITSHA1();
            String buildDate = application.getBuildDate();
            String buildUsername = application.getBuildUsername();

            scope.setExtra("Master", String.valueOf(isMasterRelease));
            scope.setExtra("Publish", String.valueOf(isBuildPublish));
            scope.setExtra("Engine Commit", engineGITSHA1);
            scope.setExtra("Build Date", buildDate);
            scope.setExtra("Build Username", buildUsername);
        });

        String sessionId = application.getSessionId();

        User user = new User();
        user.setId(sessionId);

        Sentry.setUser(user);
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        //Empty
    }

    @Override
    public void onMengineCreateApplication(MengineActivity activity) {
        Sentry.configureScope(scope -> {
            boolean isDevelopmentMode = activity.isDevelopmentMode();
            String companyName = activity.getCompanyName();
            String projectName = activity.getProjectName();
            int projectVersion = activity.getProjectVersion();

            scope.setExtra("Development", String.valueOf(isDevelopmentMode));
            scope.setExtra("Company", companyName);
            scope.setExtra("Project", projectName);
            scope.setExtra("Version", String.valueOf(projectVersion));
        });
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
        if (value == null) {
            Sentry.setExtra(key, "null");
        } else {
            Sentry.setExtra(key, String.valueOf(value));
        }
    }

    public void recordException(Throwable throwable) {
        this.logMessage("recordException throwable: %s"
            , throwable.getMessage()
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
