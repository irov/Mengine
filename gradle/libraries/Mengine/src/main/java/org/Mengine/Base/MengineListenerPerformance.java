package org.Mengine.Base;

import java.util.Map;

public interface MengineListenerPerformance extends MengineServiceInterface {
    void onMenginePerformanceTraceStart(MengineApplication application, MenginePerformanceTrace trace, String name, Map<String, String> attributes);
}
