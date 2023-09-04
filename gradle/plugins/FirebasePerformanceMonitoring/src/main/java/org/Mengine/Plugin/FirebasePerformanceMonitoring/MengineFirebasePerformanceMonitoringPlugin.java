package org.Mengine.Plugin.FirebasePerformanceMonitoring;

import com.google.firebase.perf.FirebasePerformance;
import com.google.firebase.perf.metrics.Trace;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginEngineListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineFirebasePerformanceMonitoringPlugin extends MenginePlugin implements MenginePluginApplicationListener, MenginePluginEngineListener {
    public static final String PLUGIN_NAME = "FirebasePerformanceMonitoring";
    public static final boolean PLUGIN_EMBEDDING = true;

    private static final String PLUGIN_STARTUP_TRACE = "mng_startup_trace";

    private Trace m_startupTrace;

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        m_startupTrace = FirebasePerformance.getInstance()
            .newTrace(PLUGIN_STARTUP_TRACE);

        m_startupTrace.start();
    }

    @Override
    public void onAppTerminate(MengineApplication application) {
        m_startupTrace = null;
    }

    @Override
    public void onMenginePlatformRun(MengineActivity activity) {
        m_startupTrace.stop();
    }
}
