package org.Mengine.Base;

public class MengineAnalytics {
    private static native void AndroidAnalyticsService_addContextParameterBoolean(String name, boolean value);
    private static native void AndroidAnalyticsService_addContextParameterString(String name, String value);
    private static native void AndroidAnalyticsService_addContextParameterInteger(String name, long value);
    private static native void AndroidAnalyticsService_addContextParameterDouble(String name, double value);

    public final static int EAET_CUSTOM = 0;
    public final static int EAET_EARN_VIRTUAL_CURRENCY = 1;
    public final static int EAET_SPEND_VIRTUAL_CURRENCY = 2;
    public final static int EAET_UNLOCK_ACHIEVEMENT = 3;

    private static MengineApplication m_application;
    private static final Object m_lock = new Object();

    public static void setMengineApplication(MengineApplication application) {
        MengineAnalytics.m_application = application;
    }

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
        MengineAnalyticsEventBuilder eventBuilder = new MengineAnalyticsEventBuilder(MengineAnalytics.m_application, MengineAnalytics.m_lock, name);

        return eventBuilder;
    }
}