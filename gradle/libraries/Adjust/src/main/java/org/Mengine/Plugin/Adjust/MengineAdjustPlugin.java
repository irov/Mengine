package org.Mengine.Plugin.Adjust;

import android.app.Activity;
import android.app.Application;
import android.content.Intent;
import android.os.Bundle;

import com.adjust.sdk.Adjust;
import com.adjust.sdk.AdjustConfig;
import com.adjust.sdk.AdjustEvent;
import com.adjust.sdk.BuildConfig;
import com.adjust.sdk.LogLevel;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;

import org.Mengine.Plugin.GoogleGameSocial.R;

public class MengineAdjustPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "Adjust";
    public static boolean PLUGIN_EMBEDDING = true;

    /**
     * событие
     * void eventTraking(String token)
     * событие о пукопку чего либо
     * void revenueTracking(String token, double amount, String currency)
     */

    @Override
    public void onAppCreate(MengineApplication application) {
        String environment = AdjustConfig.ENVIRONMENT_SANDBOX;
        if (!BuildConfig.DEBUG) {
            environment = AdjustConfig.ENVIRONMENT_PRODUCTION;
        }
        AdjustConfig config = new AdjustConfig(application, application.getString(R.string.adjust_app_token), environment);
        if (BuildConfig.DEBUG) {
            config.setLogLevel(LogLevel.VERBOSE);
        }

        config.setPreinstallTrackingEnabled(true);
        Adjust.onCreate(config);

        this.logInfo("adjust inited  user id = '%s'"
            , Adjust.getAdid()
        );
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        Adjust.gdprForgetMe(activity);
    }

    @Override
    public void onResume(MengineActivity activity) {
        Adjust.onResume();
    }

    @Override
    public void onPause(MengineActivity activity) {
        Adjust.onPause();
    }

    void eventTraking(String token) {
        AdjustEvent adjustEvent = new AdjustEvent(token);
        Adjust.trackEvent(adjustEvent);
    }

    void revenueTracking(String token, double amount, String currency) {
        AdjustEvent adjustEvent = new AdjustEvent(token);
        adjustEvent.setRevenue(amount, currency);
        Adjust.trackEvent(adjustEvent);
    }

}
