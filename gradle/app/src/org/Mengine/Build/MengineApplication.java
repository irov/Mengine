package org.Mengine.Build;

import android.app.ActivityManager;
import android.app.Application;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.content.Context;
import android.content.res.Configuration;
import android.os.Process;


import com.mar.sdk.MARSDK;
import com.mar.sdk.log.Log;

import java.util.Iterator;

public class MengineApplication extends Application {
    public MengineApplication() {
    }

    @Override
    public void onCreate() {
        super.onCreate();

        MARSDK.getInstance().onAppCreateAll(this);
        MARSDK.getInstance().onAppCreate(this);
    }

    @Override
    public void attachBaseContext(Context base) {
        super.attachBaseContext(base);

        MARSDK.getInstance().onAppAttachBaseContext(this, base);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        MARSDK.getInstance().onAppConfigurationChanged(this, newConfig);
    }

    @Override
    public void onTerminate() {
        super.onTerminate();

        MARSDK.getInstance().onTerminate();
    }
}