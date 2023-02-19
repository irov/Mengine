package org.Mengine.Base;

public class MengineAnalyticsEventBuilder {
    private static native void AndroidAnalyticsService_addEventBuilderParameterBoolean(Object ptr, String name, boolean value);
    private static native void AndroidAnalyticsService_addEventBuilderParameterString(Object ptr, String name, String value);
    private static native void AndroidAnalyticsService_addEventBuilderParameterInteger(Object ptr, String name, long value);
    private static native void AndroidAnalyticsService_addEventBuilderParameterDouble(Object ptr, String name, double value);
    private static native void AndroidAnalyticsService_logEventBuilder(Object ptr);

    private Object m_ptr;

    MengineAnalyticsEventBuilder(Object ptr) {
m_ptr = ptr;
    }

    public MengineAnalyticsEventBuilder addParameterBoolean(String name, boolean value) {
        AndroidAnalyticsService_addEventBuilderParameterBoolean(m_ptr, name, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterString(String name, String value) {
        AndroidAnalyticsService_addEventBuilderParameterString(m_ptr, name, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterInteger(String name, long value) {
        AndroidAnalyticsService_addEventBuilderParameterInteger(m_ptr, name, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterDouble(String name, double value) {
        AndroidAnalyticsService_addEventBuilderParameterDouble(m_ptr, name, value);

        return this;
    }

    public void log() {
        AndroidAnalyticsService_logEventBuilder(m_ptr);
        m_ptr = null;
    }
}