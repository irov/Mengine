package org.Mengine.Plugin.FirebasePerformanceMonitoring;

import android.content.Context;
import android.content.res.Configuration;

import com.google.firebase.perf.FirebasePerformance;
import com.google.firebase.perf.metrics.Trace;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginLoggerListener;

import java.util.Map;


public class MengineFirebasePerformanceMonitoringPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "FirebasePerformanceMonitoring";
    public static final boolean PLUGIN_EMBEDDING = true;

    private String STARTUP_TRACE = "startup_trace";

    private Trace m_startupTrace;

    @Override
    public void onAppCreate(MengineApplication application) {
        m_startupTrace = FirebasePerformance.getInstance().newTrace(STARTUP_TRACE);
    }

    @Override
    public void onAppTerminate(MengineApplication application) {
        m_startupTrace = null;
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
    public void onMengineApplicationReady(MengineActivity activity) {
        m_startupTrace.stop();
    }
}
