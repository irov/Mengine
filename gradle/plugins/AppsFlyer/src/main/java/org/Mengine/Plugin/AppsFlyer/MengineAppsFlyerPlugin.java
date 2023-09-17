package org.Mengine.Plugin.AppsFlyer;

import android.content.Context;

import com.appsflyer.AppsFlyerLib;
import com.appsflyer.adrevenue.adnetworks.AppsFlyerAdNetworkEventType;
import com.appsflyer.adrevenue.adnetworks.generic.MediationNetwork;
import com.appsflyer.adrevenue.adnetworks.generic.Scheme;
import com.appsflyer.attribution.AppsFlyerRequestListener;
import com.appsflyer.adrevenue.AppsFlyerAdRevenue;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdRevenueParam;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MengineInAppPurchaseParam;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginAdRevenueListener;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import java.util.Currency;
import java.util.HashMap;
import java.util.Map;

public class MengineAppsFlyerPlugin extends MenginePlugin implements MenginePluginAnalyticsListener, MenginePluginAdRevenueListener, MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "AppsFlyer";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_API_KEY = "mengine.appsflyer.api_key";

    @Override
    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_SESSION_ID) {
            String sessionId = (String)args[0];

            AppsFlyerLib appsFlyer = AppsFlyerLib.getInstance();
            appsFlyer.setCustomerUserId(sessionId);
        }
    }

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String MengineAppsFlyerPlugin_ApiKey = application.getMetaDataString(PLUGIN_METADATA_API_KEY);

        if (MengineAppsFlyerPlugin_ApiKey == null) {
            this.invalidInitialize("invalid setup meta data [%s]", PLUGIN_METADATA_API_KEY);

            return;
        }

        Context context = application.getApplicationContext();

        AppsFlyerLib appsFlyer = AppsFlyerLib.getInstance();

        if (BuildConfig.DEBUG == true) {
            appsFlyer.setDebugLog(true);
        }

        appsFlyer.init(MengineAppsFlyerPlugin_ApiKey, null, context);
        appsFlyer.start(context);

        AppsFlyerAdRevenue.Builder afRevenueBuilder = new AppsFlyerAdRevenue.Builder(application);
        AppsFlyerAdRevenue.initialize(afRevenueBuilder.build());
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        switch (eventType) {
            case EAET_CUSTOM: {
                Map<String, Object> params = new HashMap<>();

                params.putAll(bases);
                params.putAll(parameters);

                Context context = application.getApplicationContext();

                AppsFlyerLib appsFlyer = AppsFlyerLib.getInstance();
                appsFlyer.logEvent(context, eventName, params, new AppsFlyerRequestListener() {
                        @Override
                        public void onSuccess() {
                            //Empty
                        }

                        @Override
                        public void onError(int errorCode, String errorMessage) {
                            MengineAppsFlyerPlugin.this.logInfo("logEvent [CUSTOM] eventName: %s params: %s [ERROR] code: %d description: %s"
                                , eventName
                                , params
                                , errorCode
                                , errorMessage
                            );
                        }
                    });
            } break;
            case EAET_EARN_VIRTUAL_CURRENCY: {
                //ToDo
            } break;
            case EAET_SPEND_VIRTUAL_CURRENCY: {
                //ToDo
            } break;
            case EAET_UNLOCK_ACHIEVEMENT: {
                //ToDo
            } break;
            case EAET_LEVEL_UP: {
                //ToDo
            } break;
            case EAET_LEVEL_START: {
                //ToDo
            } break;
            case EAET_LEVEL_END: {
                //ToDo
            } break;
            case EAET_SELECT_ITEM: {
                //ToDo
            } break;
            case EAET_TUTORIAL_BEGIN: {
                //ToDo
            } break;
            case EAET_TUTORIAL_COMPLETE: {
                //ToDo
            } break;
            case EAET_SCREEN_VIEW: {
                //ToDo
            } break;
            default: {
                this.logWarning("event: %s unknown type: %d"
                    , eventName
                    , eventType
                );
            } break;
        }
    }

    @Override
    public void onMengineAnalyticsFlush(MengineApplication application) {
        //Empty
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
        } else if (adType == MengineAdFormat.ADFORMAT_REWARDED_INTERSTITIAL) {
            return AppsFlyerAdNetworkEventType.REWARDED.toString();
        } else if (adType == MengineAdFormat.ADFORMAT_APP_OPEN) {
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
    public void onMengineAdRevenue(MengineApplication application, Map<MengineAdRevenueParam, Object> paid) {
        MengineAdMediation mediation = (MengineAdMediation)paid.get(MengineAdRevenueParam.ADREVENUE_MEDIATION);
        MediationNetwork AppsFlyerSource = MengineAppsFlyerPlugin.getMediationNetwork(mediation);
        String network = (String)paid.get(MengineAdRevenueParam.ADREVENUE_NETWORK);
        MengineAdFormat adFormat = (MengineAdFormat)paid.get(MengineAdRevenueParam.ADREVENUE_FORMAT);
        String AppsFlyerAdType = MengineAppsFlyerPlugin.getAppsFlyerAdType(adFormat);
        String adUnitId = (String)paid.get(MengineAdRevenueParam.ADREVENUE_ADUNITID);
        String placement = (String)paid.get(MengineAdRevenueParam.ADREVENUE_PLACEMENT);
        double revenue = (double)paid.get(MengineAdRevenueParam.ADREVENUE_REVENUE_VALUE);
        String revenuePrecision = (String)paid.get(MengineAdRevenueParam.ADREVENUE_REVENUE_PRECISION);
        String revenueСurrency = (String)paid.get(MengineAdRevenueParam.ADREVENUE_REVENUE_CURRENCY);

        Map<String, String> params = new HashMap<>();
        params.put(Scheme.AD_UNIT, adUnitId);
        params.put(Scheme.AD_TYPE, AppsFlyerAdType);
        params.put(Scheme.PLACEMENT, placement);

        Currency currency = Currency.getInstance(revenueСurrency);

        AppsFlyerAdRevenue.logAdRevenue(network, AppsFlyerSource, currency, revenue, params);
    }
}
