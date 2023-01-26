package org.Mengine.Plugin.FirebaseCrashlytics;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;

import androidx.annotation.NonNull;

import com.google.firebase.crashlytics.FirebaseCrashlytics;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;


public class MengineFirebaseCrashlyticsPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "FirebaseCrashlytics";
    public static boolean PLUGIN_EMBEDDING = true;

    /**
     * <p>
     * void recordException(Throwable throwable)
     * void recordLog(String msg)
     */

    @Override
    public void onAppCreate(MengineApplication application) {
        if (BuildConfig.DEBUG == false) {
            return;
        }

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

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        activity.addLoggerPlugin(this);
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        activity.removeLoggerPlugin(this);
    }


    public void recordException(Throwable throwable) {
        this.logInfo("recordException throwable: %s"
            , throwable.getLocalizedMessage()
        );

        FirebaseCrashlytics.getInstance().recordException(throwable);
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

    public void onMengineLogger(String category, int level, int filter, int color, String msg) {
        if (level > LM_ERROR) {
            return;
        }

        FirebaseCrashlytics.getInstance().log(msg);
    }
}
