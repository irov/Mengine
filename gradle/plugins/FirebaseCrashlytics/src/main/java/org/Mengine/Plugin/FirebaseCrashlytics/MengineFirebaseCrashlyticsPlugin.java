package org.Mengine.Plugin.FirebaseCrashlytics;

import android.os.Bundle;

import com.google.firebase.crashlytics.FirebaseCrashlytics;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFatalErrorException;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginEngineListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MenginePluginLoggerListener;
import org.Mengine.Base.MenginePluginSessionIdListener;
import org.Mengine.Base.MengineUtils;

public class MengineFirebaseCrashlyticsPlugin extends MenginePlugin implements MenginePluginLoggerListener, MenginePluginApplicationListener, MenginePluginActivityListener, MenginePluginEngineListener, MenginePluginSessionIdListener {
    public static final String PLUGIN_NAME = "MengineFBCrashlytics";
    public static final boolean PLUGIN_EMBEDDING = true;

    @Override
    public void onAppInit(MengineApplication application, boolean isMainProcess) throws MenginePluginInvalidInitializeException {
        FirebaseCrashlytics.getInstance().setCustomKey("is_dev", BuildConfig.DEBUG);

        boolean isBuildPublish = application.isBuildPublish();

        if (isBuildPublish == false && android.os.Debug.isDebuggerConnected() == false) {
            FirebaseCrashlytics.getInstance().setCrashlyticsCollectionEnabled(false);
        }
    }

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String sessionId = application.getSessionId();
        FirebaseCrashlytics.getInstance().setUserId(sessionId);
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        if (BuildConfig.DEBUG == true) {
            if (FirebaseCrashlytics.getInstance().didCrashOnPreviousExecution() == true) {
                MengineUtils.makeToastDelayed(activity, "Last launch ended in a crash", 10000L);
            }
        }
    }

    @Override
    public void onMengineCaughtException(MengineApplication activity, Throwable throwable) {
        throwable.printStackTrace(System.err);

        this.recordException(throwable);
    }

    @Override
    public void onState(MengineApplication application, String name, Object value) {
        this.setCustomKey(name, value);
    }

    @Override
    public void onMengineSetSessionId(MengineApplication application, String sessionId) {
        FirebaseCrashlytics.getInstance().setUserId(sessionId);
    }

    @Override
    public void onMengineRemoveSessionData(MengineApplication application) {
        FirebaseCrashlytics.getInstance().setUserId("");
    }

    public void recordException(Throwable throwable) {
        this.logMessage("recordException throwable: %s"
            , throwable.getMessage()
        );

        FirebaseCrashlytics.getInstance().recordException(throwable);
    }

    public void setCustomKey(String key, Object value) {
        if (value == null) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, "null");
        } else if (value instanceof Boolean) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, (Boolean)value);
        } else if (value instanceof Integer) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, (Integer)value);
        } else if (value instanceof Long) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, (Long)value);
        } else if (value instanceof Float) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, (Float)value);
        } else if (value instanceof Double) {
            FirebaseCrashlytics.getInstance().setCustomKey(key, (Double)value);
        } else if (value instanceof String) {
            String stringValue = (String)value;

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
        this.logMessage("recordLog msg: %s"
            , msg
        );

        FirebaseCrashlytics.getInstance().log(msg);
    }

    public void testCrash() {
        this.logMessage("testCrash");

        this.setCustomKey("test.crash", true);

        throw new RuntimeException("Firebase Crashlytics Test Crash");
    }

    @Override
    public void onMengineLogger(MengineApplication application, int level, int filter, String category, String msg) {
        if (BuildConfig.DEBUG == true) {
            return;
        }

        switch (level) {
            case MengineLog.LM_MESSAGE_RELEASE: {
                FirebaseCrashlytics.getInstance().log("[" + category + "] R " + msg);
            }break;
            case MengineLog.LM_ERROR: {
                if (MengineLog.isFilter(filter, MengineLog.LFILTER_EXCEPTION) == true) {
                    FirebaseCrashlytics.getInstance().recordException(new MengineFatalErrorException(msg));
                } else {
                    FirebaseCrashlytics.getInstance().log("[" + category + "] E " + msg);
                }
            }break;
            case MengineLog.LM_FATAL: {
                FirebaseCrashlytics.getInstance().recordException(new MengineFatalErrorException(msg));
            }break;
        }
    }
}
