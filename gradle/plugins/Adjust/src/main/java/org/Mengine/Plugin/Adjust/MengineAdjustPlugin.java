package org.Mengine.Plugin.Adjust;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

import com.adjust.sdk.Adjust;
import com.adjust.sdk.AdjustAdRevenue;
import com.adjust.sdk.AdjustConfig;
import com.adjust.sdk.AdjustDeeplink;
import com.adjust.sdk.AdjustEvent;
import com.adjust.sdk.AdjustThirdPartySharing;
import com.adjust.sdk.LogLevel;
import com.adjust.sdk.OnDeferredDeeplinkResponseListener;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineParamAdRevenue;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerAdRevenue;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerPushToken;
import org.Mengine.Base.MengineListenerRemoteMessage;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineUtils;

public class MengineAdjustPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity, MengineListenerAdRevenue, MengineListenerTransparencyConsent, MengineListenerRemoteMessage, MengineListenerPushToken {
    public static final String SERVICE_NAME = "Adjust";
    public static final boolean SERVICE_EMBEDDING = true;
    public static final int SAVE_VERSION = 1;

    public static final @StringRes int METADATA_APP_TOKEN = R.string.mengine_adjust_app_token;

    private static final class AdjustLifecycleCallbacks implements Application.ActivityLifecycleCallbacks {
        @Override
        public void onActivityCreated(@NonNull Activity activity, Bundle savedInstanceState) {
            //Empty
        }

        @Override
        public void onActivityStarted(@NonNull Activity activity) {
            //Empty
        }

        @Override
        public void onActivityResumed(@NonNull Activity activity) {
            Adjust.onResume();
        }

        @Override
        public void onActivityPaused(@NonNull Activity activity) {
            Adjust.onPause();
        }

        @Override
        public void onActivityStopped(@NonNull Activity activity) {
            //Empty
        }

        @Override
        public void onActivitySaveInstanceState(@NonNull Activity activity, @NonNull Bundle outState) {
            //Empty
        }

        @Override
        public void onActivityDestroyed(@NonNull Activity activity) {
            //Empty
        }
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        String environment = (BuildConfig.DEBUG == true) ? AdjustConfig.ENVIRONMENT_SANDBOX : AdjustConfig.ENVIRONMENT_PRODUCTION;

        String MengineAdjustPlugin_AppToken = this.getResourceString(METADATA_APP_TOKEN);

        this.logInfo("%s: %s"
            , METADATA_APP_TOKEN
            , MengineUtils.getRedactedValue(MengineAdjustPlugin_AppToken)
        );

        AdjustConfig config = new AdjustConfig(application, MengineAdjustPlugin_AppToken, environment);

        if (BuildConfig.DEBUG == true) {
            config.setLogLevel(LogLevel.INFO);
        }

        Adjust.getAdid(adid -> {
            this.logInfo("Adjust adid: %s"
                , MengineUtils.getRedactedValue(adid)
            );

            application.setADID(adid);
        });

        config.setOnAttributionChangedListener(attribution -> {
            this.logInfo("Adjust attribution changed: %s"
                , attribution
            );

            if (attribution == null) {
                return;
            }

            String network = attribution.network;
            String campaign = attribution.campaign;

            application.setAcquisitionCampaign(network, campaign);
        });

        config.setOnEventTrackingSucceededListener(eventSuccess -> {
            this.logInfo("Adjust event tracking succeeded: %s"
                , eventSuccess
            );

            //ToDo
        });

        config.setOnEventTrackingFailedListener(eventFailure -> {
            this.logInfo("Adjust event tracking failed: %s"
                , eventFailure
            );

            //ToDo
        });

        config.setOnSessionTrackingSucceededListener(sessionSuccess -> {
            this.logInfo("Adjust session tracking succeeded: %s"
                , sessionSuccess
            );

            //ToDo
        });

        config.setOnSessionTrackingFailedListener(sessionFailure -> {
            this.logInfo("Adjust session tracking failed: %s"
                , sessionFailure
            );

            //ToDo
        });

        config.setOnDeferredDeeplinkResponseListener(deeplink -> {
            this.logInfo("Adjust deferred deep link callback called: %s"
                , deeplink
            );

            return true;
        });

        Adjust.initSdk(config);

        application.registerActivityLifecycleCallbacks(new MengineAdjustPlugin.AdjustLifecycleCallbacks());
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        Intent intent = activity.getIntent();
        if (intent != null) {
            this.onNewIntent(activity, intent);
        }
    }

    @Override
    public void onNewIntent(@NonNull MengineActivity activity, Intent intent) {
        Uri data = intent.getData();
        if (data != null) {
            AdjustDeeplink deeplink = new AdjustDeeplink(data);
            Context context = activity.getApplicationContext();

            Adjust.processDeeplink(deeplink, context);
        }
    }

    private static String getAdjustMediation(@NonNull MengineAdMediation adMediation) {
        if (adMediation == MengineAdMediation.ADMEDIATION_APPLOVINMAX) {
            return "applovin_max_sdk";
        }

        return "publisher_sdk";
    }

    @Override
    public void onMengineChangePushToken(@NonNull MengineApplication application, String token) {
        final Context context = application.getApplicationContext();

        Adjust.setPushToken(token, context);
    }

    @Override
    public void onMengineAdRevenue(@NonNull MengineApplication application, @NonNull MengineParamAdRevenue revenue) {
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
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, @NonNull MengineParamTransparencyConsent tcParam) {
        boolean EEA = tcParam.isEEA();
        boolean AD_PERSONALIZATION = tcParam.getConsentAdPersonalization();
        boolean AD_USER_DATA = tcParam.getConsentAdUserData();

        AdjustThirdPartySharing adjustThirdPartySharing = new AdjustThirdPartySharing(null);
        adjustThirdPartySharing.addGranularOption("google_dma", "eea", EEA ? "1" : "0");
        adjustThirdPartySharing.addGranularOption("google_dma", "ad_personalization", AD_PERSONALIZATION ? "1" : "0");
        adjustThirdPartySharing.addGranularOption("google_dma", "ad_user_data", AD_USER_DATA ? "1" : "0");

        this.logInfo("AdjustThirdPartySharing: %s"
            , adjustThirdPartySharing.getGranularOptions()
        );

        Adjust.trackThirdPartySharing(adjustThirdPartySharing);
    }

    @Override
    public void onMengineRemoteMessageNewToken(@NonNull MengineApplication application, String token) {
        final Context context = application.getApplicationContext();

        Adjust.setPushToken(token, context);
    }

    public void eventTracking(String token) {
        this.logInfo("eventTracking token: %s"
            , token
        );

        AdjustEvent adjustEvent = new AdjustEvent(token);
        Adjust.trackEvent(adjustEvent);
    }

    public void revenueTracking(String token, double amount, String currency) {
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