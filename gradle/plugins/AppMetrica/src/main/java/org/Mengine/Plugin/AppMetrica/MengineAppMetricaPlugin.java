package org.Mengine.Plugin.AppMetrica;

import android.content.Context;

import androidx.annotation.BoolRes;
import androidx.annotation.IntegerRes;
import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

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
import org.Mengine.Base.MengineListenerEngine;
import org.Mengine.Base.MengineListenerLogger;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineParamLoggerException;
import org.Mengine.Base.MengineParamLoggerMessage;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.Currency;
import java.util.HashMap;
import java.util.Map;

import io.appmetrica.analytics.AdRevenue;
import io.appmetrica.analytics.AdType;
import io.appmetrica.analytics.AppMetrica;
import io.appmetrica.analytics.AppMetricaConfig;

public class MengineAppMetricaPlugin extends MengineService implements MengineListenerLogger, MengineListenerAnalytics, MengineListenerAdRevenue, MengineListenerApplication, MengineListenerEngine, MengineListenerUser {
    public static final String SERVICE_NAME = "AppMetrica";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final @StringRes int METADATA_API_KEY = R.string.mengine_appmetrica_api_key;
    public static final @BoolRes int METADATA_CRASH_REPORTING = R.bool.mengine_appmetrica_crash_reporting;
    public static final @BoolRes int METADATA_NATIVE_CRASH_REPORTING = R.bool.mengine_appmetrica_native_crash_reporting;
    public static final @BoolRes int METADATA_LOCATION_TRACKING = R.bool.mengine_appmetrica_location_tracking;
    public static final @IntegerRes int METADATA_SESSION_TIMEOUT = R.integer.mengine_appmetrica_session_timeout;
    public static final @BoolRes int METADATA_LOGS = R.bool.mengine_appmetrica_logs;
    public static final @BoolRes int METADATA_HANDLE_FIRST_ACTIVATION_AS_UPDATE = R.bool.mengine_appmetrica_handle_first_activation_as_update;

    @Override
    public void onAppInit(@NonNull MengineApplication application, boolean isMainProcess) throws MengineServiceInvalidInitializeException {
        String MengineAppMetricaPlugin_ApiKey = this.getResourceString(METADATA_API_KEY);

        this.logInfo("%s: %s"
            , METADATA_API_KEY
            , MengineUtils.getRedactedValue(MengineAppMetricaPlugin_ApiKey)
        );

        String AppVersion = application.getVersionName();

        boolean MengineAppMetricaPlugin_CrashReporting = this.getResourceBoolean(METADATA_CRASH_REPORTING);
        boolean MengineAppMetricaPlugin_NativeCrashReporting = this.getResourceBoolean(METADATA_NATIVE_CRASH_REPORTING);
        int MengineAppMetricaPlugin_SessionTimeout = this.getResourceInteger(METADATA_SESSION_TIMEOUT);
        boolean MengineAppMetricaPlugin_LocationTracking = this.getResourceBoolean(METADATA_LOCATION_TRACKING);
        boolean MengineAppMetricaPlugin_Logs = this.getResourceBoolean(METADATA_LOGS);
        boolean MengineAppMetricaPlugin_HandleFirstActivationAsUpdate = this.getResourceBoolean(METADATA_HANDLE_FIRST_ACTIVATION_AS_UPDATE);

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
    public void onAppPrepare(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        String userId = application.getUserId();
        AppMetrica.setUserProfileID(userId);
    }

    @Override
    public void onMengineChangeUserId(@NonNull MengineApplication application, String oldUserId, String newUserId) {
        AppMetrica.setUserProfileID(newUserId);
    }

    @Override
    public void onMengineRemoveUserData(@NonNull MengineApplication application) {
        AppMetrica.setUserProfileID(null);
        AppMetrica.clearAppEnvironment();
    }

    @Override
    public void onMengineAnalyticsEvent(@NonNull MengineApplication application, @NonNull MengineParamAnalyticsEvent param) {
        if (param.ANALYTICS_CATEGORY == MengineAnalyticsEventCategory.MengineAnalyticsEventCategory_System) {
            return;
        }

        Map<String, Object> params = new HashMap<>();

        params.putAll(param.ANALYTICS_BASES);
        params.putAll(param.ANALYTICS_PARAMETERS);

        AppMetrica.reportEvent(param.ANALYTICS_NAME, params);
    }

    @Override
    public void onMengineAnalyticsFlush(@NonNull MengineApplication application) {
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
    public void onMengineAdRevenue(@NonNull MengineApplication application, @NonNull MengineParamAdRevenue revenue) {
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
    public void onMengineCaughtException(@NonNull MengineApplication application, Throwable throwable) {
        if (BuildConfig.DEBUG == true) {
            return;
        }

        AppMetrica.reportError("Exception", throwable);
    }

    @Override
    public void onMengineLog(@NonNull MengineApplication application, @NonNull MengineParamLoggerMessage message) {
        if (BuildConfig.DEBUG == true) {
            return;
        }

        switch (message.MESSAGE_LEVEL) {
            case MengineLog.LM_ERROR:
                AppMetrica.reportError(message.MESSAGE_CATEGORY, message.MESSAGE_DATA);
                break;
            case MengineLog.LM_FATAL:
                AppMetrica.reportError(message.MESSAGE_CATEGORY, message.MESSAGE_DATA);
                break;
        }
    }

    @Override
    public void onMengineException(@NonNull MengineApplication application, @NonNull MengineParamLoggerException exception) {
        if (BuildConfig.DEBUG == true) {
            return;
        }

        AppMetrica.reportUnhandledException(exception.EXCEPTION_THROWABLE);
    }
}
