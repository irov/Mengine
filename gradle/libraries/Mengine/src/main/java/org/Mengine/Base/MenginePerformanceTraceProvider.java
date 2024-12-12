package org.Mengine.Base;

public interface MenginePerformanceTraceProvider {
    void incrementMetric(String name, long value);
    void putMetric(String name, long value);

    void putAttribute(String name, String value);

    void stop();
}