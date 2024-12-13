package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.Map;

public interface MengineListenerPerformance extends MengineServiceInterface {
    void onMenginePerformanceTraceStart(@NonNull MengineApplication application, @NonNull MenginePerformanceTrace trace, String name, Map<String, String> attributes);
}
