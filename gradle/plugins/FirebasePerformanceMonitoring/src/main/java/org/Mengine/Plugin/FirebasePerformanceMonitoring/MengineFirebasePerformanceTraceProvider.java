package org.Mengine.Plugin.FirebasePerformanceMonitoring;

import com.google.firebase.perf.FirebasePerformance;
import com.google.firebase.perf.metrics.Trace;

import org.Mengine.Base.MenginePerformanceTraceProvider;

import java.util.Map;

class MengineFirebasePerformanceTraceProvider implements MenginePerformanceTraceProvider {
    private final Trace m_trace;

    MengineFirebasePerformanceTraceProvider(String name, Map<String, String> attributes) {
        m_trace = FirebasePerformance.getInstance()
            .newTrace(name);

        for (Map.Entry<String, String> entry : attributes.entrySet()) {
            m_trace.putAttribute(entry.getKey(), entry.getValue());
        }
    }

    @Override
    public void incrementMetric(String name, long value) {
        m_trace.incrementMetric(name, value);
    }

    @Override
    public void putMetric(String name, long value) {
        m_trace.putMetric(name, value);
    }

    @Override
    public void putAttribute(String name, String value) {
        m_trace.putAttribute(name, value);
    }

    @Override
    public void stop() {
        m_trace.stop();
    }
}