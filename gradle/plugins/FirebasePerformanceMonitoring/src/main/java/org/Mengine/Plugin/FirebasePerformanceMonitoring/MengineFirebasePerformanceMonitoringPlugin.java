package org.Mengine.Plugin.FirebasePerformanceMonitoring;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePerformanceTrace;
import org.Mengine.Base.MenginePerformanceTraceProvider;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerPerformance;

import java.util.Map;

public class MengineFirebasePerformanceMonitoringPlugin extends MengineService implements MengineListenerPerformance {
    public static final String SERVICE_NAME = "MengineFBPMonitoring";
    public static final boolean SERVICE_EMBEDDING = true;

    @Override
    public void onMenginePerformanceTraceStart(MengineApplication application, MenginePerformanceTrace trace, String name, Map<String, String> attributes) {
        MenginePerformanceTraceProvider provider = new MengineFirebasePerformanceTraceProvider(name, attributes);

        trace.addProvider(provider);
    }
}
