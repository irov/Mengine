package org.Mengine.Plugin.FirebaseCrashlytics;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import com.google.firebase.crashlytics.FirebaseCrashlytics;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFatalErrorException;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineParamLoggerMessage;
import org.Mengine.Base.MengineNative;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerEngine;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineListenerLogger;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

public class MengineFirebaseCrashlyticsPlugin extends MengineService implements MengineListenerLogger, MengineListenerApplication, MengineListenerActivity, MengineListenerEngine, MengineListenerUser {
    public static final String SERVICE_NAME = "FBCrashlytics";
    public static final boolean SERVICE_EMBEDDING = true;

    @Override
    public void onAppInit(@NonNull MengineApplication application, boolean isMainProcess) throws MengineServiceInvalidInitializeException {
        FirebaseCrashlytics.getInstance().setCustomKey("is_dev", BuildConfig.DEBUG);

        boolean isBuildPublish = application.isBuildPublish();
        boolean isDebuggerConnected = MengineUtils.isDebuggerConnected();

        if (isBuildPublish == false && isDebuggerConnected == false) {
            FirebaseCrashlytics.getInstance().setCrashlyticsCollectionEnabled(false);
        }
    }

    @Override
    public void onAppPrepare(@NonNull MengineApplication application, @NonNull Map<String, String> pluginVersions) throws MengineServiceInvalidInitializeException {
        String userId = application.getUserId();
        FirebaseCrashlytics.getInstance().setUserId(userId);

        String pluginVersionsString = String.valueOf(pluginVersions);

        FirebaseCrashlytics.getInstance().setCustomKey("plugins.version", pluginVersionsString);
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        if (BuildConfig.DEBUG == true) {
            if (FirebaseCrashlytics.getInstance().didCrashOnPreviousExecution() == true) {
                MengineUtils.makeToastDelayed(activity, "Last launch ended in a crash", 10000L);
            }
        }

        boolean isCrashlyticsForceCrash = this.hasOption("firebase.crashlytics.forcecrash");

        if (isCrashlyticsForceCrash == true) {
            long firebaseCrashlyticsForceCrashDelay = this.getOptionValueLong("firebase.crashlytics.forcecrash.delay", 5000);

            MengineUtils.performOnMainThreadDelayed(() -> {
                this.testCrash();
            }, firebaseCrashlyticsForceCrashDelay);
        }
    }

    @Override
    public void onMengineCaughtException(@NonNull MengineApplication application, Throwable throwable) {
        throwable.printStackTrace(System.err);

        this.recordException(throwable);
    }

    @Override
    public void onAppState(@NonNull MengineApplication application, String name, Object value) {
        this.setCustomKey(name, value);
    }

    @Override
    public void onMengineChangeUserId(@NonNull MengineApplication application, String userId) {
        FirebaseCrashlytics.getInstance().setUserId(userId);
    }

    @Override
    public void onMengineRemoveUserData(@NonNull MengineApplication application) {
        FirebaseCrashlytics.getInstance().setUserId("");
    }

    public void recordException(Throwable throwable) {
        this.logInfo("recordException throwable: %s"
            , throwable.getMessage()
        );

        FirebaseCrashlytics.getInstance().recordException(throwable);
    }

    public void setCustomKey(@NonNull @Size(min = 1L,max = 1024L) String key, Object value) {
        if (value == null) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, "null");
        } else if (value instanceof Boolean booleanValue) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, booleanValue);
        } else if (value instanceof Integer integerValue) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, integerValue);
        } else if (value instanceof Long longValue) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, longValue);
        } else if (value instanceof Float floatValue) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, floatValue);
        } else if (value instanceof Double doubleValue) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, doubleValue);
        } else if (value instanceof String stringValue) {
            if (stringValue.length() > 1024) {
                String stringValue1024 = stringValue.substring(0, 1024);

                FirebaseCrashlytics.getInstance().setCustomKey(key, stringValue1024);
            } else {
                FirebaseCrashlytics.getInstance().setCustomKey(key, stringValue);
            }
        } else {
            this.logError("[ERROR] unsupported custom key: %s value: %s class: %s"
                , key
                , value
                , value.getClass()
            );
        }
    }

    public void recordLog(String msg) {
        this.logInfo("recordLog msg: %s"
            , msg
        );

        FirebaseCrashlytics.getInstance().log(msg);
    }

    public void testCrash() {
        this.logMessage("testCrash");

        this.setCustomKey("test.crash", true);

        MengineNative.AndroidMain_crash("Android test crash");
    }

    @Override
    public void onMengineLog(@NonNull MengineApplication application, @NonNull MengineParamLoggerMessage message) {
        if (BuildConfig.DEBUG == true) {
            return;
        }

        switch (message.MESSAGE_LEVEL) {
            case MengineLog.LM_MESSAGE_RELEASE: {
                FirebaseCrashlytics.getInstance().log("[" + message.MESSAGE_CATEGORY + "] R " + message.MESSAGE_DATA);
            }break;
            case MengineLog.LM_ERROR: {
                if (MengineLog.isFilter(message.MESSAGE_FILTER, MengineLog.LFILTER_EXCEPTION) == true) {
                    FirebaseCrashlytics.getInstance().recordException(new MengineFatalErrorException("[" + message.MESSAGE_CATEGORY + "] E " + message.MESSAGE_DATA));
                } else {
                    FirebaseCrashlytics.getInstance().log("[" + message.MESSAGE_CATEGORY + "] E " + message.MESSAGE_DATA);
                }
            }break;
            case MengineLog.LM_FATAL: {
                FirebaseCrashlytics.getInstance().recordException(new MengineFatalErrorException(message.MESSAGE_DATA));
            }break;
        }
    }
}
