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

    /**
     * событие
     * void eventTraking(String token)
     * событие о пукопку чего либо
     * void revenueTracking(String token, double amount, String currency)
     */


    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("Adjust");
    }


    @Override
    public void onAppCreate(MengineApplication application) {
        super.onAppCreate(application);

        String appToken = "{YourAppToken}";

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

        this.log("adjust inited  user id = '%s'", Adjust.getAdid());
    }


    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        super.onCreate(activity, savedInstanceState);
        Adjust.gdprForgetMe(activity);
    }

    @Override
    public void onResume(MengineActivity activity) {
        super.onResume(activity);
        Adjust.onResume();
    }

    @Override
    public void onPause(MengineActivity activity) {
        super.onPause(activity);
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
