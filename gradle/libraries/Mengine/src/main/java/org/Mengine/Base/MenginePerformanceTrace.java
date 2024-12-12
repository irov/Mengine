package org.Mengine.Base;

import java.util.ArrayList;
import java.util.List;

public class MenginePerformanceTrace {
    private List<MenginePerformanceTraceProvider> m_providers = new ArrayList<>();

    public void addProvider(MenginePerformanceTraceProvider provider) {
        m_providers.add(provider);
    }

    public void incrementMetric(String name, long value) {
        for (MenginePerformanceTraceProvider provider : m_providers) {
            provider.incrementMetric(name, value);
        }
    }

    public void putMetric(String name, long value) {
        for (MenginePerformanceTraceProvider provider : m_providers) {
            provider.putMetric(name, value);
        }
    }

    public void putAttribute(String name, String value) {
        for (MenginePerformanceTraceProvider provider : m_providers) {
            provider.putAttribute(name, value);
        }
    }

    public void stop() {
        for (MenginePerformanceTraceProvider provider : m_providers) {
            provider.stop();
        }
    }
}