package org.Mengine.Plugin.Adjust;

import android.content.Context;
import android.os.Bundle;

import com.adjust.sdk.Adjust;
import com.adjust.sdk.AdjustAdRevenue;
import com.adjust.sdk.AdjustConfig;
import com.adjust.sdk.AdjustEvent;
import com.adjust.sdk.BuildConfig;
import com.adjust.sdk.LogLevel;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import java.util.Map;

public class MengineAdjustPlugin extends MenginePlugin implements MenginePluginApplicationListener, MenginePluginActivityListener, MenginePluginAnalyticsListener {
    public static final String PLUGIN_NAME = "Adjust";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_APP_TOKEN = "mengine.adjust.app_token";

    /**
     * событие
     * void eventTraking(String token)
     * событие о пукопку чего либо
     * void revenueTracking(String token, double amount, String currency)
     */

    @Override
    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_PUSH_TOKEN) {
            final String token = (String)args[0];

            final Context context = application.getApplicationContext();

            Adjust.setPushToken(token, context);
        }
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String environment = AdjustConfig.ENVIRONMENT_PRODUCTION;

        if (BuildConfig.DEBUG == true) {
            environment = AdjustConfig.ENVIRONMENT_SANDBOX;
        }

        String MengineAdjustPlugin_AppToken = application.getMetaDataString(PLUGIN_METADATA_APP_TOKEN);

        if (MengineAdjustPlugin_AppToken == null) {
            this.invalidInitialize("invalid setup meta data [%s]", PLUGIN_METADATA_APP_TOKEN);

            return;
        }

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_APP_TOKEN
            , MengineAdjustPlugin_AppToken
        );

        AdjustConfig config = new AdjustConfig(application, MengineAdjustPlugin_AppToken, environment);

        if (BuildConfig.DEBUG == true) {
            config.setLogLevel(LogLevel.VERBOSE);
        }

        config.setPreinstallTrackingEnabled(true);
        Adjust.onCreate(config);

        this.logMessage("Adjust Adid: %s"
            , Adjust.getAdid()
        );
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
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

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        //ToDo
    }

    @Override
    public void onMengineAnalyticsFlush(MengineApplication application) {
        //ToDo
    }

    private static String getAdjustMediationNetwork(String source) {
        if (source.equalsIgnoreCase(EA_ADMEDIATION_APPLOVINMAX) == true) {
            return AdjustConfig.AD_REVENUE_APPLOVIN_MAX;
        }

        return AdjustConfig.AD_REVENUE_SOURCE_PUBLISHER;
    }

    @Override
    public void onMengineAnalyticsRevenuePaid(MengineApplication application, Map<String, Object> paid) {
        String source = (String)paid.get(EA_ADREVENUE_SOURCE);
        String AdjustSource = MengineAdjustPlugin.getAdjustMediationNetwork(source);
        String network = (String)paid.get(EA_ADREVENUE_NETWORK);
        int adType = (int)paid.get(EA_ADREVENUE_TYPE);
        String adUnitId = (String)paid.get(EA_ADREVENUE_ADUNITID);
        String placement = (String)paid.get(EA_ADREVENUE_PLACEMENT);
        double revenue = (double)paid.get(EA_ADREVENUE_REVENUE_VALUE);
        String revenuePrecision = (String)paid.get(EA_ADREVENUE_REVENUE_PRECISION);
        String revenueСurrency = (String)paid.get(EA_ADREVENUE_REVENUE_CURRENCY);

        AdjustAdRevenue adjustAdRevenue = new AdjustAdRevenue(AdjustSource);
        adjustAdRevenue.setRevenue(revenue, revenueСurrency);
        adjustAdRevenue.setAdRevenueNetwork(network);
        adjustAdRevenue.setAdRevenueUnit(adUnitId);
        adjustAdRevenue.setAdRevenuePlacement(placement);

        Adjust.trackAdRevenue(adjustAdRevenue);
    }

    public void eventTraking(String token) {
        this.logMessage("eventTraking token: %s"
            , token
        );

        AdjustEvent adjustEvent = new AdjustEvent(token);
        Adjust.trackEvent(adjustEvent);
    }

    public void revenueTracking(String token, double amount, String currency) {
        this.logMessage("revenueTracking token: %s amount: %f currency: %s"
            , token
            , amount
            , currency
        );

        AdjustEvent adjustEvent = new AdjustEvent(token);
        adjustEvent.setRevenue(amount, currency);
        Adjust.trackEvent(adjustEvent);
    }
}
