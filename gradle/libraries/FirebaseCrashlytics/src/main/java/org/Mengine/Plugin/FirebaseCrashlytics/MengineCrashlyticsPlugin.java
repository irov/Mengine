package org.Mengine.Plugin.FirebaseCrashlytics;

import android.os.Handler;

import androidx.annotation.NonNull;

import com.google.firebase.crashlytics.FirebaseCrashlytics;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;


public class MengineCrashlyticsPlugin extends MenginePlugin {

    /**
     * <p>
     * void recordException(Throwable throwable)
     *  void recordLog(String msg)
     */

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("FirebaseCrashlytics");
    }

    @Override
    public void onAppCreate(MengineApplication application) {
        if (!BuildConfig.DEBUG) {
            return;
        }

        FirebaseCrashlytics.getInstance().setCrashlyticsCollectionEnabled(false);

        Thread.setDefaultUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(@NonNull Thread thread, @NonNull Throwable throwable) {
                logError("app crash -> %s", throwable.getMessage());
                throwable.printStackTrace();

                FirebaseCrashlytics.getInstance().recordException(throwable);
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        android.os.Process.killProcess(android.os.Process.myPid());
                        System.exit(0);
                    }
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
}
