package org.Mengine.Plugin.Flurry;

import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;

import com.flurry.android.FlurryAgent;
import com.flurry.android.FlurryAgentListener;
import com.flurry.android.FlurryEventRecordStatus;
import com.flurry.android.FlurryPerformance;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineAnalyticsEventCategory;
import org.Mengine.Base.MengineParamAnalyticsEvent;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerAnalytics;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.HashMap;
import java.util.Map;

public class MengineFlurryPlugin extends MengineService implements MengineListenerAnalytics, MengineListenerApplication, MengineListenerUser, FlurryAgentListener {
    public static final String SERVICE_NAME = "Flurry";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final String METADATA_API_KEY = "mengine.flurry.api_key";

    @Override
    public void onAppPrepare(MengineApplication application, @NonNull Map<String, String> pluginVersions) throws MengineServiceInvalidInitializeException {
        String MengineFlurryPlugin_ApiKey = this.getMetaDataString(METADATA_API_KEY);

        this.logInfo("%s: %s"
            , METADATA_API_KEY
            , MengineUtils.getRedactedValue(MengineFlurryPlugin_ApiKey)
        );

        String userId = application.getUserId();
        FlurryAgent.setUserId(userId);

        Context context = application.getApplicationContext();

        FlurryAgent.Builder builder = new FlurryAgent.Builder();
        builder.withCaptureUncaughtExceptions(false);
        builder.withIncludeBackgroundSessionsInMetrics(true);
        builder.withPerformanceMetrics(FlurryPerformance.ALL);
        builder.withLogEnabled(true);

        if (BuildConfig.DEBUG == true) {
            builder.withLogLevel(Log.VERBOSE);
        } else {
            builder.withLogLevel(Log.ERROR);
        }

        builder.withListener(this);
        builder.build(context, MengineFlurryPlugin_ApiKey);
    }

    @Override
    public void onSessionStarted() {
        this.logInfo("Flurry session started");
    }

    @Override
    public void onMengineChangeUserId(@NonNull MengineApplication application, String oldUserId, String newUserId) {
        FlurryAgent.setUserId(newUserId);
    }

    @Override
    public void onMengineRemoveUserData(@NonNull MengineApplication application) {
        FlurryAgent.deleteData();
    }

    private void updateParams(Map<String, String> params, Map<String, Object> bases) {
        for (Map.Entry<String, Object> entry : bases.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();

            if (value instanceof Boolean) {
                params.put(key, String.valueOf(value));
            } else if (value instanceof Long) {
                params.put(key, String.valueOf(value));
            } else if (value instanceof Double) {
                params.put(key, String.valueOf(value));
            } else if (value instanceof String) {
                params.put(key, String.valueOf(value));
            } else {
                this.logError("[ERROR] unsupported parameter: %s class: %s"
                    , value
                    , value == null ? "null" : value.getClass()
                );

                return;
            }
        }
    }

    @Override
    public void onMengineAnalyticsEvent(@NonNull MengineApplication application, @NonNull MengineParamAnalyticsEvent param) {
        if (param.ANALYTICS_CATEGORY == MengineAnalyticsEventCategory.MengineAnalyticsEventCategory_System) {
            return;
        }

        Map<String, String> params = new HashMap<>();

        this.updateParams(params, param.ANALYTICS_BASES);
        this.updateParams(params, param.ANALYTICS_PARAMETERS);

        FlurryEventRecordStatus status = FlurryAgent.logEvent(param.ANALYTICS_NAME, params);

        switch (status) {
            case kFlurryEventFailed:
                this.logError("[ERROR] failed to log event: %s"
                    , param.ANALYTICS_NAME
                );
                break;
            case kFlurryEventRecorded:
                //Empty
                break;
            case kFlurryEventUniqueCountExceeded:
                this.logWarning("[ERROR] failed to log event: %s unique count exceeded"
                    , param.ANALYTICS_NAME
                );
                break;
            case kFlurryEventParamsCountExceeded:
                this.logWarning("[ERROR] failed to log event: %s params count exceeded"
                    , param.ANALYTICS_NAME
                );
                break;
            case kFlurryEventLogCountExceeded:
                this.logInfo("failed to log event: %s log count exceeded"
                    , param.ANALYTICS_NAME
                );
                break;
            case kFlurryEventLoggingDelayed:
                this.logInfo("failed to log event: %s logging delayed"
                    , param.ANALYTICS_NAME
                );
                break;
            case kFlurryEventAnalyticsDisabled:
                this.logDebug("failed to log event: %s analytics disabled"
                    , param.ANALYTICS_NAME
                );
                break;
            case kFlurryEventParamsMismatched:
                this.logError("failed to log event: %s params mismatched"
                    , param.ANALYTICS_NAME
                );
                break;
        }
    }
}
