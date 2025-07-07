package org.Mengine.Plugin.FirebaseCrashlytics;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import com.google.firebase.crashlytics.CustomKeysAndValues;
import com.google.firebase.crashlytics.FirebaseCrashlytics;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFatalErrorException;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineParamLoggerException;
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
import org.json.JSONArray;

import java.util.List;
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
    public void onAppPrepare(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        String userId = application.getUserId();

        if (userId != null) {
            FirebaseCrashlytics.getInstance().setUserId(userId);
        }
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        if (BuildConfig.DEBUG == true) {
            if (FirebaseCrashlytics.getInstance().didCrashOnPreviousExecution() == true) {
                MengineUtils.makeToastDelayed(activity, 10000L, "Last launch ended in a crash");
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
    public void onResume(@NonNull MengineActivity activity) {
        MengineFirebaseCrashlyticsANRMonitor.start();
    }

    @Override
    public void onPause(@NonNull MengineActivity activity) {
        MengineFirebaseCrashlyticsANRMonitor.stop();
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
    public void onMengineChangeUserId(@NonNull MengineApplication application, String oldUserId, String newUserId) {
        FirebaseCrashlytics.getInstance().setUserId(newUserId);
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

    @Override
    public void onMengineException(@NonNull MengineApplication application, @NonNull MengineParamLoggerException exception) {
        CustomKeysAndValues.Builder builder = new CustomKeysAndValues.Builder();
        builder.putString("category", exception.EXCEPTION_CATEGORY.toString());

        for (Map.Entry<String, Object> entry : exception.EXCEPTION_ATTRIBUTES.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();

            if (value == null) {
                builder.putString(key, "null");
            } else if (value instanceof Boolean booleanValue) {
                builder.putBoolean(key, booleanValue);
            } else if (value instanceof Integer integerValue) {
                builder.putInt(key, integerValue);
            } else if (value instanceof Long longValue) {
                builder.putLong(key, longValue);
            } else if (value instanceof Float floatValue) {
                builder.putFloat(key, floatValue);
            } else if (value instanceof Double doubleValue) {
                builder.putDouble(key, doubleValue);
            } else if (value instanceof String stringValue) {
                builder.putString(key, stringValue);
            } else if (value instanceof List<?> listValue) {
                JSONArray json = MengineUtils.jsonArrayFromArrayList(listValue);
                String jsonString = json.toString();
                builder.putString(key, jsonString);
            } else {
                this.logError("[ERROR] unsupported custom key: %s value: %s class: %s"
                    , key
                    , value
                    , value.getClass()
                );
            }
        }

        CustomKeysAndValues values = builder.build();

        FirebaseCrashlytics.getInstance().recordException(exception.EXCEPTION_THROWABLE, values);
    }
}
