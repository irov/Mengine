package org.Mengine.Plugin.FirebaseCrashlytics;

import android.content.Context;
import android.widget.Toast;

import com.google.firebase.crashlytics.FirebaseCrashlytics;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MenginePluginLoggerListener;
import org.Mengine.Base.MengineUtils;

public class MengineFirebaseCrashlyticsPlugin extends MenginePlugin implements MenginePluginLoggerListener, MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "FirebaseCrashlytics";
    public static final boolean PLUGIN_EMBEDDING = true;

    /**
     * <p>
     * void recordException(Throwable throwable)
     * void recordLog(String msg)
     */

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        if (BuildConfig.DEBUG == true) {
            if (FirebaseCrashlytics.getInstance().didCrashOnPreviousExecution() == true) {
                MengineUtils.performOnMainThreadDelayed(() -> {
                    Context applicatonContext = application.getApplicationContext();

                    Toast text = Toast.makeText(applicatonContext, "Last launch ended in a crash", Toast.LENGTH_LONG);
                    text.show();
                }, 5000L);
            }
        }
    }

    @Override
    public void onMengineCurrentSceneChange(MengineActivity activity, String name) {
        FirebaseCrashlytics.getInstance().setCustomKey("current.scene", name);
    }

    @Override
    public void onState(MengineApplication application, String name, Object value) {
        this.setCustomKey(name, value);
    }

    public void recordException(Throwable throwable) {
        this.logMessage("recordException throwable: %s"
            , throwable.getLocalizedMessage()
        );

        FirebaseCrashlytics.getInstance().recordException(throwable);
    }

    public void setCustomKey(String key, Object value) {
        this.logMessage("setCustomKey key: %s value: %s"
            , key
            , value
        );

        if (value instanceof Boolean) {
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
            FirebaseCrashlytics.getInstance().setCustomKey(key, (String)value);
        } else {
            this.logError("unsupported custom key: %s value: %s class: %s"
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

        FirebaseCrashlytics.getInstance().setCustomKey("TestCrash", true);

        throw new RuntimeException("Firebase Crashlytics Test Crash");
    }

    @Override
    public void onMengineLogger(MengineApplication application, String category, int level, int filter, int color, String msg) {
        if (level > MengineLog.LM_ERROR) {
            return;
        }

        FirebaseCrashlytics.getInstance().log("[" + category + "] " + msg);
    }
}
