package org.Mengine.Plugin.Adjust;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.os.Bundle;

import com.adjust.sdk.Adjust;
import com.adjust.sdk.AdjustAdRevenue;
import com.adjust.sdk.AdjustAttribution;
import com.adjust.sdk.AdjustConfig;
import com.adjust.sdk.AdjustEvent;
import com.adjust.sdk.AdjustThirdPartySharing;
import com.adjust.sdk.LogLevel;
import com.adjust.sdk.OnAdidReadListener;
import com.adjust.sdk.OnAttributionChangedListener;

import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdRevenueParam;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerAdRevenue;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerPushToken;
import org.Mengine.Base.MengineListenerRemoteMessage;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineTransparencyConsentParam;
import org.Mengine.Base.MengineUtils;

public class MengineAdjustPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity, MengineListenerAdRevenue, MengineListenerTransparencyConsent, MengineListenerRemoteMessage, MengineListenerPushToken {
    public static final String SERVICE_NAME = "Adjust";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final String METADATA_APP_TOKEN = "mengine.adjust.app_token";

    protected String m_adid;

    private static final class AdjustLifecycleCallbacks implements Application.ActivityLifecycleCallbacks {
        @Override
        public void onActivityCreated(Activity activity, Bundle savedInstanceState) {
            //Empty
        }

        @Override
        public void onActivityStarted(Activity activity) {
            //Empty
        }

        @Override
        public void onActivityResumed(Activity activity) {
            Adjust.onResume();
        }

        @Override
        public void onActivityPaused(Activity activity) {
            Adjust.onPause();
        }

        @Override
        public void onActivityStopped(Activity activity) {
            //Empty
        }

        @Override
        public void onActivitySaveInstanceState(Activity activity, Bundle outState) {
            //Empty
        }

        @Override
        public void onActivityDestroyed(Activity activity) {
            //Empty
        }
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MengineServiceInvalidInitializeException {
        String environment = AdjustConfig.ENVIRONMENT_PRODUCTION;

        if (BuildConfig.DEBUG == true) {
            environment = AdjustConfig.ENVIRONMENT_SANDBOX;
        }

        String MengineAdjustPlugin_AppToken = this.getMetaDataString(METADATA_APP_TOKEN);

        this.logInfo("%s: %s"
            , METADATA_APP_TOKEN
            , MengineUtils.getRedactedValue(MengineAdjustPlugin_AppToken)
        );

        AdjustConfig config = new AdjustConfig(application, MengineAdjustPlugin_AppToken, environment);

        if (BuildConfig.DEBUG == true) {
            config.setLogLevel(LogLevel.INFO);
        }

        Adjust.getAdid(new OnAdidReadListener() {
            @Override
            public void onAdidRead(String adid) {
                MengineAdjustPlugin.this.logInfo("Adjust adid: %s"
                    , MengineUtils.getRedactedValue(adid)
                );

                MengineAdjustPlugin.this.m_adid = adid;
            }
        });

        config.setOnAttributionChangedListener(new OnAttributionChangedListener() {
            @Override
            public void onAttributionChanged(AdjustAttribution attribution) {
                MengineAdjustPlugin.this.logInfo("Adjust attribution: %s"
                    , attribution.toString()
                );

                //ToDo Acquisition
            }
        });

        Adjust.initSdk(config);

        application.registerActivityLifecycleCallbacks(new MengineAdjustPlugin.AdjustLifecycleCallbacks());
    }

    private static String getAdjustMediation(MengineAdMediation adMediation) {
        if (adMediation == MengineAdMediation.ADMEDIATION_APPLOVINMAX) {
            return "applovin_max_sdk";
        }

        return "publisher_sdk";
    }

    @Override
    public void onMengineChangePushToken(MengineApplication application, String token) {
        final Context context = application.getApplicationContext();

        Adjust.setPushToken(token, context);
    }

    @Override
    public void onMengineAdRevenue(MengineApplication application, MengineAdRevenueParam revenue) {
        MengineAdMediation mediation = revenue.ADREVENUE_MEDIATION;
        String AdjustMediation = MengineAdjustPlugin.getAdjustMediation(mediation);
        String network = revenue.ADREVENUE_NETWORK;
        MengineAdFormat adType = revenue.ADREVENUE_FORMAT;
        String adUnitId = revenue.ADREVENUE_ADUNITID;
        String placement = revenue.ADREVENUE_PLACEMENT;
        double revenueValue = revenue.ADREVENUE_REVENUE_VALUE;
        String revenuePrecision = revenue.ADREVENUE_REVENUE_PRECISION;
        String revenueCurrency = revenue.ADREVENUE_REVENUE_CURRENCY;

        AdjustAdRevenue adjustAdRevenue = new AdjustAdRevenue(AdjustMediation);
        adjustAdRevenue.setRevenue(revenueValue, revenueCurrency);
        adjustAdRevenue.setAdRevenueNetwork(network);
        adjustAdRevenue.setAdRevenueUnit(adUnitId);
        adjustAdRevenue.setAdRevenuePlacement(placement);

        Adjust.trackAdRevenue(adjustAdRevenue);
    }

    @Override
    public void onMengineTransparencyConsent(MengineApplication application, MengineTransparencyConsentParam tcParam) {
        boolean EEA = tcParam.isEEA();
        boolean AD_PERSONALIZATION = tcParam.getConsentAdPersonalization();
        boolean AD_USER_DATA = tcParam.getConsentAdUserData();

        AdjustThirdPartySharing adjustThirdPartySharing = new AdjustThirdPartySharing(null);
        adjustThirdPartySharing.addGranularOption("google_dma", "eea", EEA ? "1" : "0");
        adjustThirdPartySharing.addGranularOption("google_dma", "ad_personalization", AD_PERSONALIZATION ? "1" : "0");
        adjustThirdPartySharing.addGranularOption("google_dma", "ad_user_data", AD_USER_DATA ? "1" : "0");

        this.logMessage("AdjustThirdPartySharing: %s"
            , adjustThirdPartySharing.getGranularOptions()
        );

        Adjust.trackThirdPartySharing(adjustThirdPartySharing);
    }

    @Override
    public void onMengineRemoteMessageNewToken(MengineApplication application, String token) {
        final Context context = application.getApplicationContext();

        Adjust.setPushToken(token, context);
    }

    public void eventTracking(String token) {
        this.logMessage("eventTracking token: %s"
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
