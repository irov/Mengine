package org.Mengine.Plugin.Sentry;

import android.content.Context;

import androidx.annotation.BoolRes;
import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerEngine;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineListenerLogger;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineParamLoggerException;
import org.Mengine.Base.MengineParamLoggerMessage;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineUtils;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import io.sentry.Sentry;
import io.sentry.android.core.SentryAndroid;
import io.sentry.protocol.User;
import io.sentry.SentryLogLevel;
import io.sentry.SentryLogParameters;
import io.sentry.SentryAttribute;
import io.sentry.SentryAttributes;

public class MengineSentryPlugin extends MengineService implements MengineListenerApplication, MengineListenerEngine, MengineListenerTransparencyConsent, MengineListenerUser, MengineListenerLogger {
    public static final String SERVICE_NAME = "Sentry";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final @StringRes int METADATA_DSN = R.string.mengine_sentry_dsn;
    public static final @BoolRes int METADATA_ENABLE_UNCAUGHT_EXCEPTION_HANDLER = R.bool.mengine_sentry_enable_uncaught_exception_handler;

    public static boolean m_passMeasurementGDPR = false;

    private static SentryLogLevel getSentryLogLevel(@NonNull MengineParamLoggerMessage message) {
        switch (message.MESSAGE_LEVEL) {
            case MengineLog.LM_FATAL:
                return SentryLogLevel.FATAL;
            case MengineLog.LM_ERROR:
                return SentryLogLevel.ERROR;
            case MengineLog.LM_WARNING:
                return SentryLogLevel.WARNING;
            case MengineLog.LM_INFO:
            case MengineLog.LM_MESSAGE:
            case MengineLog.LM_MESSAGE_RELEASE:
                return SentryLogLevel.INFO;
            case MengineLog.LM_DEBUG:
            case MengineLog.LM_VERBOSE:
                return SentryLogLevel.DEBUG;
            default:
                return SentryLogLevel.INFO;
        }
    }

    private void setCustomKey(String key, Object value) {
        if (value == null) {
            Sentry.setExtra(key, "null");
        } else {
            Sentry.setExtra(key, String.valueOf(value));
        }
    }

    private void recordException(Throwable throwable) {
        this.logInfo("recordException throwable: %s",
            throwable.getMessage()
        );

        throwable.printStackTrace(System.err);

        Sentry.captureException(throwable);
    }

    public void testCrash() {
        this.logMessage("testCrash");

        this.setCustomKey("test.crash", true);

        throw new RuntimeException("Sentry Test Crash");
    }

    @Override
    public void onAppInit(MengineApplication application, boolean isMainProcess) throws MengineServiceInvalidInitializeException {
        if (isMainProcess == false) {
            return;
        }

        String MengineSentryPlugin_DSN = this.getResourceString(METADATA_DSN);

        this.logInfo("%s: %s",
            METADATA_DSN,
            MengineUtils.getRedactedValue(MengineSentryPlugin_DSN)
        );

        boolean MengineSentryPlugin_EnableUncaughtExceptionHandler = this.getResourceBoolean(METADATA_ENABLE_UNCAUGHT_EXCEPTION_HANDLER);

        this.logInfo("%s: %b",
            METADATA_ENABLE_UNCAUGHT_EXCEPTION_HANDLER,
            MengineSentryPlugin_EnableUncaughtExceptionHandler
        );

        MengineParamTransparencyConsent tcParam = application.makeTransparencyConsentParam();

        m_passMeasurementGDPR = tcParam.getConsentMeasurement();

        this.logInfo("GDPR measurement: %s",
            m_passMeasurementGDPR
        );

        tcParam.getConsentAnalyticsStorage();

        Context context = application.getApplicationContext();

        SentryAndroid.init(context, options -> {
            options.setDsn(MengineSentryPlugin_DSN);

            if (application.isBuildPublish() == true) {
                options.setEnvironment("prod");
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
                if (m_passMeasurementGDPR == false) {
                    return null;
                }

                return event;
            });

            options.setBeforeSendTransaction((transaction, hint) -> {
                if (m_passMeasurementGDPR == false) {
                    return null;
                }

                return transaction;
            });
        });
    }

    @Override
    public void onAppPrepare(MengineApplication application) throws MengineServiceInvalidInitializeException {
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

        String userId = application.getUserId();

        User user = new User();
        user.setId(userId);

        Sentry.setUser(user);
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MengineServiceInvalidInitializeException {
        //Empty
    }

    @Override
    public void onMengineChangeUserId(@NonNull MengineApplication application, String oldUserId, String newUserId) {
        User user = new User();
        user.setId(newUserId);

        Sentry.setUser(user);
    }

    @Override
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, @NonNull MengineParamTransparencyConsent consent) {
        m_passMeasurementGDPR = consent.getConsentMeasurement();

        this.logInfo("GDPR measurement: %s",
            m_passMeasurementGDPR
        );
    }

    @Override
    public void onMengineRemoveUserData(@NonNull MengineApplication application) {
        Sentry.setUser(null);
    }

    @Override
    public void onMengineLog(@NonNull MengineApplication application, @NonNull MengineParamLoggerMessage message) {
        if (BuildConfig.DEBUG == false) {
            if (message.MESSAGE_LEVEL != MengineLog.LM_WARNING && message.MESSAGE_LEVEL != MengineLog.LM_ERROR && message.MESSAGE_LEVEL != MengineLog.LM_FATAL) {
                return;
            }
        }

        SentryLogLevel level = MengineSentryPlugin.getSentryLogLevel(message);

        List<SentryAttribute> attributesList = new ArrayList<>();
        attributesList.add(SentryAttribute.stringAttribute("log.category", message.MESSAGE_CATEGORY.toString()));
        attributesList.add(SentryAttribute.stringAttribute("log.thread", message.MESSAGE_THREAD));

        if (message.MESSAGE_FILE != null && message.MESSAGE_FILE.isEmpty() == false) {
            attributesList.add(SentryAttribute.stringAttribute("log.file", message.MESSAGE_FILE));
            attributesList.add(SentryAttribute.integerAttribute("log.line", message.MESSAGE_LINE));
        }

        if (message.MESSAGE_FUNCTION != null && message.MESSAGE_FUNCTION.isEmpty() == false) {
            attributesList.add(SentryAttribute.stringAttribute("log.function", message.MESSAGE_FUNCTION));
        }

        SentryAttributes attributes = SentryAttributes.of(attributesList.toArray(new SentryAttribute[0]));

        Sentry.logger().log(level, SentryLogParameters.create(attributes), message.MESSAGE_DATA);
    }

    @Override
    public void onMengineException(@NonNull MengineApplication application, @NonNull MengineParamLoggerException exception) {
        Sentry.withScope(scope -> {
            scope.setExtra("log.category", exception.EXCEPTION_CATEGORY.toString());

            for (Map.Entry<String, Object> entry : exception.EXCEPTION_ATTRIBUTES.entrySet()) {
                String key = entry.getKey();
                Object value = entry.getValue();

                if (value == null) {
                    scope.setExtra(key, "null");
                } else {
                    scope.setExtra(key, String.valueOf(value));
                }
            }

            this.recordException(exception.EXCEPTION_THROWABLE);
        });
    }

    @Override
    public void onMengineCaughtException(@NonNull MengineApplication application, Throwable throwable) {
        this.recordException(throwable);
    }

    @Override
    public void onAppState(@NonNull MengineApplication application, String name, Object value) {
        this.setCustomKey("." + name, value);
    }
}

