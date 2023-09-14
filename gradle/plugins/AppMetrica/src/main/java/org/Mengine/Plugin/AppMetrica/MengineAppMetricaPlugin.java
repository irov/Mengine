package org.Mengine.Plugin.AppMetrica;

import android.content.Context;

import com.yandex.metrica.AdRevenue;
import com.yandex.metrica.AdType;
import com.yandex.metrica.YandexMetrica;
import com.yandex.metrica.YandexMetricaConfig;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginEngineListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MenginePluginLoggerListener;

import java.util.Currency;
import java.util.HashMap;
import java.util.Map;

public class MengineAppMetricaPlugin extends MenginePlugin implements MenginePluginLoggerListener, MenginePluginAnalyticsListener, MenginePluginApplicationListener, MenginePluginEngineListener {
    public static final String PLUGIN_NAME = "AppMetrica";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_API_KEY = "mengine.appmetrica.api_key";
    public static final String PLUGIN_METADATA_CRASH_REPORTING = "mengine.appmetrica.crash_reporting";
    public static final String PLUGIN_METADATA_NATIVE_CRASH_REPORTING = "mengine.appmetrica.native_crash_reporting";
    public static final String PLUGIN_METADATA_LOCATION_TRACKING = "mengine.appmetrica.location_tracking";
    public static final String PLUGIN_METADATA_SESSION_TIMEOUT = "mengine.appmetrica.session_timeout";
    public static final String PLUGIN_METADATA_LOGS = "mengine.appmetrica.logs";
    public static final String PLUGIN_METADATA_HANDLE_FIRST_ACTIVATION_AS_UPDATE = "mengine.appmetrica.handle_first_activation_as_update";
    public static final String PLUGIN_METADATA_STATISTICS_SENDING = "mengine.appmetrica.statistics_sending";

    /**
     * void logEvent (String name, Bundle params)
     */

    @Override
    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_SESSION_ID) {
            String sessionId = (String)args[0];

            YandexMetrica.setUserProfileID(sessionId);
        }
    }

    @Override
    public void onAppInit(MengineApplication application, boolean isMainProcess) throws MenginePluginInvalidInitializeException {
        String MengineAppMetricaPlugin_ApiKey = application.getMetaDataString(PLUGIN_METADATA_API_KEY);

        if (MengineAppMetricaPlugin_ApiKey == null) {
            this.invalidInitialize("invalid setup meta data [%s]", PLUGIN_METADATA_API_KEY);

            return;
        }

        boolean MengineAppMetricaPlugin_CrashReporting = application.getMetaDataBoolean(PLUGIN_METADATA_CRASH_REPORTING, true);
        boolean MengineAppMetricaPlugin_NativeCrashReporting = application.getMetaDataBoolean(PLUGIN_METADATA_NATIVE_CRASH_REPORTING, true);
        int MengineAppMetricaPlugin_SessionTimeout = application.getMetaDataInteger(PLUGIN_METADATA_SESSION_TIMEOUT, 300);
        boolean MengineAppMetricaPlugin_LocationTracking = application.getMetaDataBoolean(PLUGIN_METADATA_LOCATION_TRACKING, false);
        boolean MengineAppMetricaPlugin_Logs = application.getMetaDataBoolean(PLUGIN_METADATA_LOGS, true);
        boolean MengineAppMetricaPlugin_HandleFirstActivationAsUpdate = application.getMetaDataBoolean(PLUGIN_METADATA_HANDLE_FIRST_ACTIVATION_AS_UPDATE, false);
        boolean MengineAppMetricaPlugin_StatisticSending = application.getMetaDataBoolean(PLUGIN_METADATA_STATISTICS_SENDING, true);

        YandexMetricaConfig.Builder builder = YandexMetricaConfig
            .newConfigBuilder(MengineAppMetricaPlugin_ApiKey)
                .withCrashReporting(MengineAppMetricaPlugin_CrashReporting)
                .withNativeCrashReporting(MengineAppMetricaPlugin_NativeCrashReporting)
                .withSessionTimeout(MengineAppMetricaPlugin_SessionTimeout)
                .withLocationTracking(MengineAppMetricaPlugin_LocationTracking)
                .handleFirstActivationAsUpdate(MengineAppMetricaPlugin_HandleFirstActivationAsUpdate)
                .withStatisticsSending(MengineAppMetricaPlugin_StatisticSending)
                .withRevenueAutoTrackingEnabled(false)
                ;

        if (BuildConfig.DEBUG == true) {
            if (MengineAppMetricaPlugin_Logs == true) {
                builder.withLogs();
            }
        }

        YandexMetricaConfig config = builder.build();

        Context context = application.getApplicationContext();

        YandexMetrica.activate(context, config);
        YandexMetrica.enableActivityAutoTracking(application);
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        switch (eventType) {
            case EAET_CUSTOM: {
                Map<String, Object> params = new HashMap<>();

                params.putAll(bases);
                params.putAll(parameters);

                YandexMetrica.reportEvent(eventName, params);
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
        YandexMetrica.sendEventsBuffer();
    }

    private static AdType getYandexAdType(int adType) {
        if (adType == EA_ADTYPE_BANNER) {
            return AdType.BANNER;
        } else if (adType == EA_ADTYPE_LEADER) {
            return AdType.BANNER;
        } else if (adType == EA_ADTYPE_INTERSTITIAL) {
            return AdType.INTERSTITIAL;
        } else if (adType == EA_ADTYPE_REWARDED) {
            return AdType.REWARDED;
        } else if (adType == EA_ADTYPE_REWARDED_INTERSTITIAL) {
            return AdType.REWARDED;
        } else if (adType == EA_ADTYPE_MREC) {
            return AdType.MREC;
        } else if (adType == EA_ADTYPE_NATIVE) {
            return AdType.NATIVE;
        }

        return AdType.OTHER;
    }

    @Override
    public void onMengineAnalyticsRevenuePaid(MengineApplication application, Map<String, Object> paid) {
        String source = (String)paid.get(EA_ADREVENUE_SOURCE);
        String networkName = (String)paid.get(EA_ADREVENUE_NETWORK);
        int adType = (int)paid.get(EA_ADREVENUE_TYPE);
        AdType YandexAdType = MengineAppMetricaPlugin.getYandexAdType(adType);
        String adUnitId = (String)paid.get(EA_ADREVENUE_ADUNITID);
        String placement = (String)paid.get(EA_ADREVENUE_PLACEMENT);
        double revenue = (double)paid.get(EA_ADREVENUE_REVENUE_VALUE);
        String revenuePrecision = (String)paid.get(EA_ADREVENUE_REVENUE_PRECISION);
        String currency = (String)paid.get(EA_ADREVENUE_REVENUE_CURRENCY);

        AdRevenue adRevenue = AdRevenue.newBuilder(revenue, Currency.getInstance(currency))
            .withAdNetwork(networkName)
            .withAdPlacementId(placement)
            .withAdType(YandexAdType)
            .withAdUnitId(adUnitId)
            .withPrecision(revenuePrecision)
            .build();

        YandexMetrica.reportAdRevenue(adRevenue);
    }

    @Override
    public void onMengineCaughtException(MengineApplication activity, Throwable throwable) {
        if (BuildConfig.DEBUG == true) {
            return;
        }

        YandexMetrica.reportError("Exception", throwable);
    }

    @Override
    public void onMengineLogger(MengineApplication application, String category, int level, int filter, int color, String msg) {
        if (BuildConfig.DEBUG == true) {
            return;
        }

        switch (level) {
            case MengineLog.LM_ERROR:
                YandexMetrica.reportError(category, msg);
                break;
            case MengineLog.LM_FATAL:
                YandexMetrica.reportError(category, msg);
                break;
        }
    }
}
