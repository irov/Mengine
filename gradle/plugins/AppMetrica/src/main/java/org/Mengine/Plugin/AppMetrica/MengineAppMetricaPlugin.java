package org.Mengine.Plugin.AppMetrica;

import android.content.Context;

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

import io.appmetrica.analytics.AdRevenue;
import io.appmetrica.analytics.AdType;
import io.appmetrica.analytics.AppMetrica;
import io.appmetrica.analytics.AppMetricaConfig;

public class MengineAppMetricaPlugin extends MenginePlugin implements MenginePluginLoggerListener, MenginePluginAnalyticsListener, MenginePluginAdRevenueListener, MenginePluginApplicationListener, MenginePluginEngineListener, MenginePluginSessionIdListener {
    public static final String SERVICE_NAME = "AppMetrica";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final String METADATA_API_KEY = "mengine.appmetrica.api_key";
    public static final String METADATA_CRASH_REPORTING = "mengine.appmetrica.crash_reporting";
    public static final String METADATA_NATIVE_CRASH_REPORTING = "mengine.appmetrica.native_crash_reporting";
    public static final String METADATA_LOCATION_TRACKING = "mengine.appmetrica.location_tracking";
    public static final String METADATA_SESSION_TIMEOUT = "mengine.appmetrica.session_timeout";
    public static final String METADATA_LOGS = "mengine.appmetrica.logs";
    public static final String METADATA_HANDLE_FIRST_ACTIVATION_AS_UPDATE = "mengine.appmetrica.handle_first_activation_as_update";

    @Override
    public void onAppInit(MengineApplication application, boolean isMainProcess) throws MenginePluginInvalidInitializeException {
        String MengineAppMetricaPlugin_ApiKey = this.getMetaDataString(METADATA_API_KEY);

        this.logInfo("%s: %s"
            , METADATA_API_KEY
            , MengineUtils.getRedactedValue(MengineAppMetricaPlugin_ApiKey)
        );

        String AppVersion = application.getVersionName();

        boolean MengineAppMetricaPlugin_CrashReporting = this.getMetaDataBoolean(METADATA_CRASH_REPORTING);
        boolean MengineAppMetricaPlugin_NativeCrashReporting = this.getMetaDataBoolean(METADATA_NATIVE_CRASH_REPORTING);
        int MengineAppMetricaPlugin_SessionTimeout = this.getMetaDataInteger(METADATA_SESSION_TIMEOUT);
        boolean MengineAppMetricaPlugin_LocationTracking = this.getMetaDataBoolean(METADATA_LOCATION_TRACKING);
        boolean MengineAppMetricaPlugin_Logs = this.getMetaDataBoolean(METADATA_LOGS);
        boolean MengineAppMetricaPlugin_HandleFirstActivationAsUpdate = this.getMetaDataBoolean(METADATA_HANDLE_FIRST_ACTIVATION_AS_UPDATE);

        AppMetricaConfig.Builder builder = AppMetricaConfig
            .newConfigBuilder(MengineAppMetricaPlugin_ApiKey)
                .withAppVersion(AppVersion)
                .withCrashReporting(MengineAppMetricaPlugin_CrashReporting)
                .withNativeCrashReporting(MengineAppMetricaPlugin_NativeCrashReporting)
                .withSessionTimeout(MengineAppMetricaPlugin_SessionTimeout)
                .withLocationTracking(MengineAppMetricaPlugin_LocationTracking)
                .handleFirstActivationAsUpdate(MengineAppMetricaPlugin_HandleFirstActivationAsUpdate)
                .withRevenueAutoTrackingEnabled(false)
                ;

        if (MengineAppMetricaPlugin_Logs == true) {
            builder.withLogs();
        }

        AppMetricaConfig config = builder.build();

        Context context = application.getApplicationContext();

        AppMetrica.activate(context, config);
        AppMetrica.enableActivityAutoTracking(application);
    }

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String sessionId = application.getSessionId();
        AppMetrica.setUserProfileID(sessionId);
    }

    @Override
    void onMengineSessionId(MengineApplication application, String sessionId) {
        AppMetrica.setUserProfileID(sessionId);
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        Map<String, Object> params = new HashMap<>();

        params.putAll(bases);
        params.putAll(parameters);

        AppMetrica.reportEvent(eventName, params);
    }

    @Override
    public void onMengineAnalyticsFlush(MengineApplication application) {
        AppMetrica.sendEventsBuffer();
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

        AppMetrica.reportAdRevenue(adRevenue);
    }

    @Override
    public void onMengineCaughtException(MengineApplication application, Throwable throwable) {
        if (BuildConfig.DEBUG == true) {
            return;
        }

        AppMetrica.reportError("Exception", throwable);
    }

    @Override
    public void onMengineLogger(@NonNull MengineApplication application, int level, int filter, String category, String msg) {
        if (BuildConfig.DEBUG == true) {
            return;
        }

        switch (level) {
            case MengineLog.LM_ERROR:
                AppMetrica.reportError(category, msg);
                break;
            case MengineLog.LM_FATAL:
                AppMetrica.reportError(category, msg);
                break;
        }
    }
}
