package org.Mengine.Plugin.AppsFlyer;

import android.content.Context;

import androidx.annotation.NonNull;

import com.appsflyer.AppsFlyerLib;
import com.appsflyer.adrevenue.adnetworks.AppsFlyerAdNetworkEventType;
import com.appsflyer.adrevenue.adnetworks.generic.MediationNetwork;
import com.appsflyer.adrevenue.adnetworks.generic.Scheme;
import com.appsflyer.attribution.AppsFlyerRequestListener;
import com.appsflyer.adrevenue.AppsFlyerAdRevenue;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineParamAdRevenue;
import org.Mengine.Base.MengineAnalyticsEventCategory;
import org.Mengine.Base.MengineParamAnalyticsEvent;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerAdRevenue;
import org.Mengine.Base.MengineListenerAnalytics;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.Currency;
import java.util.HashMap;
import java.util.Map;

public class MengineAppsFlyerPlugin extends MengineService implements MengineListenerAnalytics, MengineListenerAdRevenue, MengineListenerApplication, MengineListenerUser {
    public static final String SERVICE_NAME = "AppsFlyer";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final String METADATA_API_KEY = "mengine.appsflyer.api_key";

    @Override
    public void onAppPrepare(@NonNull MengineApplication application, @NonNull Map<String, String> pluginVersions) throws MengineServiceInvalidInitializeException {
        String MengineAppsFlyerPlugin_ApiKey = application.getMetaDataString(METADATA_API_KEY);

        this.logInfo("%s: %s"
            , METADATA_API_KEY
            , MengineUtils.getRedactedValue(MengineAppsFlyerPlugin_ApiKey)
        );

        Context context = application.getApplicationContext();

        AppsFlyerLib appsFlyer = AppsFlyerLib.getInstance();

        if (BuildConfig.DEBUG == true) {
            appsFlyer.setDebugLog(true);
        }

        String userId = application.getUserId();
        appsFlyer.setCustomerUserId(userId);

        appsFlyer.init(MengineAppsFlyerPlugin_ApiKey, null, context);
        appsFlyer.start(context);

        AppsFlyerAdRevenue.Builder afRevenueBuilder = new AppsFlyerAdRevenue.Builder(application);
        AppsFlyerAdRevenue.initialize(afRevenueBuilder.build());
    }

    @Override
    public void onMengineChangeUserId(@NonNull MengineApplication application, String userId) {
        AppsFlyerLib appsFlyer = AppsFlyerLib.getInstance();
        appsFlyer.setCustomerUserId(userId);
    }

    @Override
    public void onMengineRemoveUserData(@NonNull MengineApplication application) {
        AppsFlyerLib appsFlyer = AppsFlyerLib.getInstance();
        appsFlyer.setCustomerUserId(null);
    }

    @Override
    public void onMengineAnalyticsEvent(@NonNull MengineApplication application, @NonNull MengineParamAnalyticsEvent param) {
        if (param.ANALYTICS_CATEGORY == MengineAnalyticsEventCategory.MengineAnalyticsEventCategory_System) {
            return;
        }

        Map<String, Object> params = new HashMap<>();

        params.putAll(param.ANALYTICS_BASES);
        params.putAll(param.ANALYTICS_PARAMETERS);

        Context context = application.getApplicationContext();

        AppsFlyerLib appsFlyer = AppsFlyerLib.getInstance();
        appsFlyer.logEvent(context, param.ANALYTICS_NAME, params, new AppsFlyerRequestListener() {
                @Override
                public void onSuccess() {
                    //Empty
                }

                @Override
                public void onError(int errorCode, String errorMessage) {
                    MengineAppsFlyerPlugin.this.logInfo("logEvent [CUSTOM] eventName: %s params: %s [ERROR] code: %d description: %s"
                        , param.ANALYTICS_NAME
                        , params
                        , errorCode
                        , errorMessage
                    );
                }
            });
    }

    private static String getAppsFlyerAdType(MengineAdFormat adType) {
        if (adType == MengineAdFormat.ADFORMAT_BANNER) {
            return AppsFlyerAdNetworkEventType.BANNER.toString();
        } else if (adType == MengineAdFormat.ADFORMAT_LEADER) {
            return AppsFlyerAdNetworkEventType.BANNER.toString();
        } else if (adType == MengineAdFormat.ADFORMAT_INTERSTITIAL) {
            return AppsFlyerAdNetworkEventType.INTERSTITIAL.toString();
        } else if (adType == MengineAdFormat.ADFORMAT_REWARDED) {
            return AppsFlyerAdNetworkEventType.REWARDED.toString();
        } else if (adType == MengineAdFormat.ADFORMAT_APPOPEN) {
            return AppsFlyerAdNetworkEventType.APP_OPEN.toString();
        } else if (adType == MengineAdFormat.ADFORMAT_NATIVE) {
            return AppsFlyerAdNetworkEventType.NATIVE.toString();
        }

        return "UNKNOWN";
    }

    private static MediationNetwork getMediationNetwork(MengineAdMediation adMediation) {
        if (adMediation == MengineAdMediation.ADMEDIATION_APPLOVINMAX) {
            return MediationNetwork.applovinmax;
        }

        return MediationNetwork.customMediation;
    }

    @Override
    public void onMengineAdRevenue(@NonNull MengineApplication application, @NonNull MengineParamAdRevenue revenue) {
        MengineAdMediation mediation = revenue.ADREVENUE_MEDIATION;
        MediationNetwork AppsFlyerSource = MengineAppsFlyerPlugin.getMediationNetwork(mediation);
        String network = revenue.ADREVENUE_NETWORK;
        MengineAdFormat adFormat = revenue.ADREVENUE_FORMAT;
        String AppsFlyerAdType = MengineAppsFlyerPlugin.getAppsFlyerAdType(adFormat);
        String adUnitId = revenue.ADREVENUE_ADUNITID;
        String placement = revenue.ADREVENUE_PLACEMENT;
        double revenueValue = revenue.ADREVENUE_REVENUE_VALUE;
        String revenuePrecision = revenue.ADREVENUE_REVENUE_PRECISION;
        String revenueCurrency = revenue.ADREVENUE_REVENUE_CURRENCY;

        Map<String, String> params = new HashMap<>();
        params.put(Scheme.AD_UNIT, adUnitId);
        params.put(Scheme.AD_TYPE, AppsFlyerAdType);
        params.put(Scheme.PLACEMENT, placement);

        Currency currency = Currency.getInstance(revenueCurrency);

        AppsFlyerAdRevenue.logAdRevenue(network, AppsFlyerSource, currency, revenueValue, params);
    }
}
