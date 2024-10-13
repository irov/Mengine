package org.Mengine.Plugin.Flurry;

import android.content.Context;
import android.util.Log;

import com.flurry.android.FlurryAgent;
import com.flurry.android.FlurryAgentListener;
import com.flurry.android.FlurryEventRecordStatus;
import com.flurry.android.FlurryPerformance;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import java.util.HashMap;
import java.util.Map;

public class MengineFlurryPlugin extends MenginePlugin implements MenginePluginAnalyticsListener, MenginePluginApplicationListener, MenginePluginSessionIdListener, FlurryAgentListener {
    public static final String PLUGIN_NAME = "MengineFlurry";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_API_KEY = "mengine.flurry.api_key";

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String MengineFlurryPlugin_ApiKey = this.getMetaDataString(PLUGIN_METADATA_API_KEY);

        String sessionId = application.getSessionId();
        FlurryAgent.setUserId(sessionId);

        Context context = application.getApplicationContext();

        FlurryAgent.Builder builder = new FlurryAgent.Builder();
        builder.withCaptureUncaughtExceptions(false);
        builder.withIncludeBackgroundSessionsInMetrics(true);
        builder.withPerformanceMetrics(FlurryPerformance.ALL);
        builder.withLogEnabled(true);

        if (BuildConfig.DEBUG) {
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
    void onMengineSessionId(MengineApplication application, String sessionId) {
        FlurryAgent.setUserId(sessionId);
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        Map<String, String> params = new HashMap<>();

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

        for (Map.Entry<String, Object> entry : parameters.entrySet()) {
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

        FlurryEventRecordStatus status = FlurryAgent.logEvent(eventName, params);

        if (status != FlurryEventRecordStatus.kFlurryEventRecorded) {
            this.logError("[ERROR] failed to log event: %s status: %s"
                , eventName
                , status
            );
        }
    }
}
