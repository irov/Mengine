package org.Mengine.Plugin.FirebaseCrashlytics;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;

import androidx.annotation.NonNull;

import com.google.firebase.crashlytics.FirebaseCrashlytics;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginLoggerListener;
import org.Mengine.Base.MenginePlugin;


public class MengineFirebaseCrashlyticsPlugin extends MenginePlugin implements MenginePluginLoggerListener, MenginePluginApplicationListener {
    public static String PLUGIN_NAME = "FirebaseCrashlytics";
    public static boolean PLUGIN_EMBEDDING = true;

    /**
     * <p>
     * void recordException(Throwable throwable)
     * void recordLog(String msg)
     */

    @Override
    public void onAppCreate(MengineApplication application) {
        if (BuildConfig.DEBUG == true) {
            FirebaseCrashlytics.getInstance().setCrashlyticsCollectionEnabled(false);

            Thread.setDefaultUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
                @Override
                public void uncaughtException(@NonNull Thread thread, @NonNull Throwable throwable) {
                    MengineFirebaseCrashlyticsPlugin.this.logError("app crash -> %s", throwable.getMessage());
                    throwable.printStackTrace();

                    FirebaseCrashlytics.getInstance().recordException(throwable);

                    new Handler(Looper.getMainLooper()).postDelayed(() -> {
                        android.os.Process.killProcess(android.os.Process.myPid());
                        System.exit(0);
                    }, 1000L);
                }
            });
        }

        FirebaseCrashlytics.getInstance().setCustomKey("onCreate", false);
        FirebaseCrashlytics.getInstance().setCustomKey("onDestroy", false);
    }

    @Override
    public void onAppTerminate(MengineApplication application) {
    }

    @Override
    public void onAppAttachBaseContext(MengineApplication application, Context base) {
    }

    @Override
    public void onAppConfigurationChanged(MengineApplication application, Configuration newConfig) {
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        FirebaseCrashlytics.getInstance().setCustomKey("onCreate", true);
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        FirebaseCrashlytics.getInstance().setCustomKey("onDestroy", true);
    }

    @Override
    public void onPause(MengineActivity activity) {
        FirebaseCrashlytics.getInstance().setCustomKey("onPause", true);
    }

    @Override
    public void onResume(MengineActivity activity) {
        FirebaseCrashlytics.getInstance().setCustomKey("onPause", false);
    }

    public void recordException(Throwable throwable) {
        this.logInfo("recordException throwable: %s"
            , throwable.getLocalizedMessage()
        );

        FirebaseCrashlytics.getInstance().recordException(throwable);
    }

    public void setCustomKey(String key, Object value) {
        this.logInfo("setCustomKey key: %s value: %s"
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
            this.logError("unsupported custom key [%s] value [%s] %s"
                , key
                , value.getClass()
                , value
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
        this.logError("testCrash");

        FirebaseCrashlytics.getInstance().setCustomKey("TestCrash", true);

        throw new RuntimeException("Firebase Crashlytics Test Crash");
    }

    @Override
    public void onMengineLogger(MengineActivity activity, String category, int level, int filter, int color, String msg) {
        if (level > LM_ERROR) {
            return;
        }

        FirebaseCrashlytics.getInstance().log(msg);
    }
}
