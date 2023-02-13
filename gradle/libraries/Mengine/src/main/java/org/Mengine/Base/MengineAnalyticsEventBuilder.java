package org.Mengine.Base;

public class MengineAnalyticsEventBuilder {
    private static native Object AndroidAnalyticsService_buildEvent(String name);

    private static native void AndroidAnalyticsService_addParameterBoolean(Object ptr, String name, boolean value);
    private static native void AndroidAnalyticsService_addParameterString(Object ptr, String name, String value);
    private static native void AndroidAnalyticsService_addParameterInteger(Object ptr, String name, long value);
    private static native void AndroidAnalyticsService_addParameterDouble(Object ptr, String name, double value);
    private static native void AndroidAnalyticsService_log(Object ptr);

    private Object m_ptr;

    MengineAnalyticsEventBuilder(String name) {
        m_ptr = AndroidAnalyticsService_buildEvent(name);
    }

    public MengineAnalyticsEventBuilder addParameterBoolean( String name, boolean value ) {
        AndroidAnalyticsService_addParameterBoolean(m_ptr, name, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterString( String name, String value ) {
        AndroidAnalyticsService_addParameterString(m_ptr, name, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterInteger( String name, long value ) {
        AndroidAnalyticsService_addParameterInteger(m_ptr, name, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterDouble( String name, double value ) {
        AndroidAnalyticsService_addParameterDouble(m_ptr, name, value);

        return this;
    }

    public void log() {
        AndroidAnalyticsService_log(m_ptr);
        m_ptr = null;
    }
}