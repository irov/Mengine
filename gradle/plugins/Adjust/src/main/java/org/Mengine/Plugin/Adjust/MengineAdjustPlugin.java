package org.Mengine.Plugin.Adjust;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Bundle;

import com.adjust.sdk.Adjust;
import com.adjust.sdk.AdjustConfig;
import com.adjust.sdk.AdjustEvent;
import com.adjust.sdk.BuildConfig;
import com.adjust.sdk.LogLevel;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;

public class MengineAdjustPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static String PLUGIN_NAME = "Adjust";
    public static boolean PLUGIN_EMBEDDING = true;

    /**
     * событие
     * void eventTraking(String token)
     * событие о пукопку чего либо
     * void revenueTracking(String token, double amount, String currency)
     */

    @Override
    public void onEvent(MengineActivity activity, String id, Object ... args) {
        if (id.equals("PushToken") == true) {
            String token = (String)args[0];

            final Context context = activity.getApplicationContext();

            Adjust.setPushToken(token, context);
        }
    }

    @Override
    public void onAppCreate(MengineApplication application) {
        String environment = AdjustConfig.ENVIRONMENT_PRODUCTION;

        if (BuildConfig.DEBUG == true) {
            environment = AdjustConfig.ENVIRONMENT_SANDBOX;
        }

        String adjust_app_token = application.getMetaDataString("mengine.adjust.app_token");

        AdjustConfig config = new AdjustConfig(application, adjust_app_token, environment);

        if (BuildConfig.DEBUG) {
            config.setLogLevel(LogLevel.VERBOSE);
        }

        config.setPreinstallTrackingEnabled(true);
        Adjust.onCreate(config);

        this.logInfo("Adjust Adid: %s"
            , Adjust.getAdid()
        );
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
        this.logInfo("eventTraking token: %s"
            , token
        );

        AdjustEvent adjustEvent = new AdjustEvent(token);
        Adjust.trackEvent(adjustEvent);
    }

    void revenueTracking(String token, double amount, String currency) {
        this.logInfo("revenueTracking token: %s amount: %f currency: %s"
            , token
            , amount
            , currency
        );

        AdjustEvent adjustEvent = new AdjustEvent(token);
        adjustEvent.setRevenue(amount, currency);
        Adjust.trackEvent(adjustEvent);
    }
}
