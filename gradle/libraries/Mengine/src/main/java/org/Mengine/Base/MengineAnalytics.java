package org.Mengine.Base;

public class MengineAnalytics {
    private static native Object AndroidAnalyticsService_buildEvent(String name);
    private static native void AndroidAnalyticsService_addContextParameterBoolean(String name, boolean value);
    private static native void AndroidAnalyticsService_addContextParameterString(String name, String value);
    private static native void AndroidAnalyticsService_addContextParameterInteger(String name, long value);
    private static native void AndroidAnalyticsService_addContextParameterDouble(String name, double value);

    static public void addContextParameterBoolean(String name, boolean value) {
        AndroidAnalyticsService_addContextParameterBoolean(name, value);
    }

    static public void addContextParameterString(String name, String value) {
        AndroidAnalyticsService_addContextParameterString(name, value);
    }

    static public void addContextParameterInteger(String name, long value) {
        AndroidAnalyticsService_addContextParameterInteger(name, value);
    }

    static public void addContextParameterDouble(String name, double value) {
        AndroidAnalyticsService_addContextParameterDouble(name, value);
    }

    static public MengineAnalyticsEventBuilder buildEvent(String name) {
        Object ptr = AndroidAnalyticsService_buildEvent(name);

        MengineAnalyticsEventBuilder eventBuilder = new MengineAnalyticsEventBuilder(ptr);

        return eventBuilder;
    }
}