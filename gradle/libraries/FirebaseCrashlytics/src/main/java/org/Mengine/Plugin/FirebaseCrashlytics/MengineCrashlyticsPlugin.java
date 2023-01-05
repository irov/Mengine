package org.Mengine.Plugin.FirebaseCrashlytics;

import android.os.Handler;
import android.os.Looper;

import androidx.annotation.NonNull;

import com.google.firebase.crashlytics.FirebaseCrashlytics;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;


public class MengineCrashlyticsPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "FirebaseCrashlytics";
    public static boolean PLUGIN_EMBEDDING = true;

    /**
     * <p>
     * void recordException(Throwable throwable)
     * void recordLog(String msg)
     */

    @Override
    public void onAppCreate(MengineApplication application) {
        if (!BuildConfig.DEBUG) {
            return;
        }

        FirebaseCrashlytics.getInstance().setCrashlyticsCollectionEnabled(false);

        Thread.setDefaultUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(@NonNull Thread thread, @NonNull Throwable throwable) {
                MengineCrashlyticsPlugin.this.logError("app crash -> %s", throwable.getMessage());
                throwable.printStackTrace();

                FirebaseCrashlytics.getInstance().recordException(throwable);

                new Handler(Looper.getMainLooper()).postDelayed(() -> {
                    android.os.Process.killProcess(android.os.Process.myPid());
                    System.exit(0);
                }, 1000L);
            }
        });
    }

    public void recordException(Throwable throwable) {
        FirebaseCrashlytics.getInstance().recordException(throwable);
    }

    public void recordLog(String msg) {
        FirebaseCrashlytics.getInstance().log(msg);
    }

    public void testCrash() {
        FirebaseCrashlytics.getInstance().setCustomKey("TestCrash", true);

        throw new RuntimeException("Firebase Crashlytics Test Crash");
    }
}
