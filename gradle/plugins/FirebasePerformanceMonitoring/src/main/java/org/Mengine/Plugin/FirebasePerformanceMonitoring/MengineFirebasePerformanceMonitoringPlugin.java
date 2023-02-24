package org.Mengine.Plugin.FirebasePerformanceMonitoring;

import android.content.Context;
import android.content.res.Configuration;

import com.google.firebase.perf.FirebasePerformance;
import com.google.firebase.perf.metrics.Trace;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginLoggerListener;

import java.util.Map;


public class MengineFirebasePerformanceMonitoringPlugin extends MenginePlugin implements MenginePluginLoggerListener, MenginePluginAnalyticsListener, MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "FirebasePerformanceMonitoring";
    public static final boolean PLUGIN_EMBEDDING = true;

    private String LIVE_TRACE = "live_trace";

    private String ANALYTICS_EVENT_COUNT = "analytics_event_count";
    private String ERROR_LOG_COUNT = "error_log_count";

    private Trace m_liveTrace;

    @Override
    public void onAppCreate(MengineApplication application) {
        m_liveTrace = FirebasePerformance.getInstance().newTrace(LIVE_TRACE);
    }

    @Override
    public void onAppTerminate(MengineApplication application) {
        m_liveTrace.stop();
    }

    @Override
    public void onAppAttachBaseContext(MengineApplication application, Context base) {
        //Empty
    }

    @Override
    public void onAppConfigurationChanged(MengineApplication application, Configuration newConfig) {
        //Empty
    }

    @Override
    public void onMengineLogger(MengineApplication application, String category, int level, int filter, int color, String msg) {
        if (level > MengineLog.LM_ERROR) {
            return;
        }

        m_liveTrace.incrementMetric(ERROR_LOG_COUNT, 1);
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> parameters) {
        m_liveTrace.incrementMetric(ANALYTICS_EVENT_COUNT, 1);
    }
}
