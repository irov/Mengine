package org.Mengine.Plugin.AppMetrica;

import android.content.Context;

import com.yandex.metrica.AdRevenue;
import com.yandex.metrica.AdType;
import com.yandex.metrica.YandexMetrica;
import com.yandex.metrica.YandexMetricaConfig;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdRevenueParam;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginAdRevenueListener;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginEngineListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MenginePluginLoggerListener;

import java.util.Currency;
import java.util.HashMap;
import java.util.Map;

public class MengineAppMetricaPlugin extends MenginePlugin implements MenginePluginLoggerListener, MenginePluginAnalyticsListener, MenginePluginAdRevenueListener, MenginePluginApplicationListener, MenginePluginEngineListener {
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
        String MengineAppMetricaPlugin_ApiKey = this.getMetaDataString(PLUGIN_METADATA_API_KEY);

        String AppVersion = application.getVersionName();

        boolean MengineAppMetricaPlugin_CrashReporting = this.getMetaDataBoolean(PLUGIN_METADATA_CRASH_REPORTING);
        boolean MengineAppMetricaPlugin_NativeCrashReporting = this.getMetaDataBoolean(PLUGIN_METADATA_NATIVE_CRASH_REPORTING);
        int MengineAppMetricaPlugin_SessionTimeout = this.getMetaDataInteger(PLUGIN_METADATA_SESSION_TIMEOUT);
        boolean MengineAppMetricaPlugin_LocationTracking = this.getMetaDataBoolean(PLUGIN_METADATA_LOCATION_TRACKING);
        boolean MengineAppMetricaPlugin_Logs = this.getMetaDataBoolean(PLUGIN_METADATA_LOGS);
        boolean MengineAppMetricaPlugin_HandleFirstActivationAsUpdate = this.getMetaDataBoolean(PLUGIN_METADATA_HANDLE_FIRST_ACTIVATION_AS_UPDATE);
        boolean MengineAppMetricaPlugin_StatisticSending = this.getMetaDataBoolean(PLUGIN_METADATA_STATISTICS_SENDING);

        YandexMetricaConfig.Builder builder = YandexMetricaConfig
            .newConfigBuilder(MengineAppMetricaPlugin_ApiKey)
                .withAppVersion(AppVersion)
                .withCrashReporting(MengineAppMetricaPlugin_CrashReporting)
                .withNativeCrashReporting(MengineAppMetricaPlugin_NativeCrashReporting)
                .withSessionTimeout(MengineAppMetricaPlugin_SessionTimeout)
                .withLocationTracking(MengineAppMetricaPlugin_LocationTracking)
                .handleFirstActivationAsUpdate(MengineAppMetricaPlugin_HandleFirstActivationAsUpdate)
                .withStatisticsSending(MengineAppMetricaPlugin_StatisticSending)
                .withRevenueAutoTrackingEnabled(false)
                ;

        if (MengineAppMetricaPlugin_Logs == true) {
            builder.withLogs();
        }

        YandexMetricaConfig config = builder.build();

        Context context = application.getApplicationContext();

        YandexMetrica.activate(context, config);
        YandexMetrica.enableActivityAutoTracking(application);
    }

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String sessionId = application.getSessionId();
        YandexMetrica.setUserProfileID(sessionId);
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        Map<String, Object> params = new HashMap<>();

        params.putAll(bases);
        params.putAll(parameters);

        YandexMetrica.reportEvent(eventName, params);
    }

    @Override
    public void onMengineAnalyticsFlush(MengineApplication application) {
        YandexMetrica.sendEventsBuffer();
    }

    private static AdType getYandexAdType(MengineAdFormat adType) {
        if (adType == MengineAdFormat.ADFORMAT_BANNER) {
            return AdType.BANNER;
        } else if (adType == MengineAdFormat.ADFORMAT_LEADER) {
            return AdType.BANNER;
        } else if (adType == MengineAdFormat.ADFORMAT_INTERSTITIAL) {
            return AdType.INTERSTITIAL;
        } else if (adType == MengineAdFormat.ADFORMAT_REWARDED) {
            return AdType.REWARDED;
        } else if (adType == MengineAdFormat.ADFORMAT_REWARDED_INTERSTITIAL) {
            return AdType.REWARDED;
        } else if (adType == MengineAdFormat.ADFORMAT_MREC) {
            return AdType.MREC;
        } else if (adType == MengineAdFormat.ADFORMAT_NATIVE) {
            return AdType.NATIVE;
        }

        return AdType.OTHER;
    }

    @Override
    public void onMengineAdRevenue(MengineApplication application, MengineAdRevenueParam revenue) {
        MengineAdMediation mediation = revenue.ADREVENUE_MEDIATION;
        String networkName = revenue.ADREVENUE_NETWORK;
        MengineAdFormat adFormat = revenue.ADREVENUE_FORMAT;
        AdType YandexAdType = MengineAppMetricaPlugin.getYandexAdType(adFormat);
        String adUnitId = revenue.ADREVENUE_ADUNITID;
        String placement = revenue.ADREVENUE_PLACEMENT;
        double revenueValue = revenue.ADREVENUE_REVENUE_VALUE;
        String revenuePrecision = revenue.ADREVENUE_REVENUE_PRECISION;
        String revenueCurrency = revenue.ADREVENUE_REVENUE_CURRENCY;

        Currency currency = Currency.getInstance(revenueCurrency);

        AdRevenue adRevenue = AdRevenue.newBuilder(revenueValue, currency)
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
    public void onMengineLogger(MengineApplication application, int level, String category, String msg) {
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
