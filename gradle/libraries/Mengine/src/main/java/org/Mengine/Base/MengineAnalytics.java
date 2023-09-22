package org.Mengine.Base;

import androidx.annotation.Size;

import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

public class MengineAnalytics {
    public final static int EAET_CUSTOM = 0;
    public final static int EAET_EARN_VIRTUAL_CURRENCY = 1;
    public final static int EAET_SPEND_VIRTUAL_CURRENCY = 2;
    public final static int EAET_UNLOCK_ACHIEVEMENT = 3;
    public final static int EAET_LEVEL_UP = 4;
    public final static int EAET_LEVEL_START = 5;
    public final static int EAET_LEVEL_END = 6;
    public final static int EAET_SELECT_ITEM = 7;
    public final static int EAET_TUTORIAL_BEGIN = 8;
    public final static int EAET_TUTORIAL_COMPLETE = 9;
    public final static int EAET_SCREEN_VIEW = 10;

    private static MengineApplication m_application;
    private static Map<String, Object> m_context = new HashMap<>();
    private static Map<String, Object> m_getter = new HashMap<>();

    public static void setMengineApplication(MengineApplication application) {
        MengineAnalytics.m_application = application;
    }

    static private void assertContext(String key) {
        if (MengineAnalytics.m_context.containsKey(key) == true) {
            String msg = String.format(Locale.US, "global analytics context parameter '%s' already exist", key);
            throw new AssertionError(msg);
        }
    }

    static private void assertGetter(String key) {
        if (MengineAnalytics.m_getter.containsKey(key) == true) {
            String msg = String.format(Locale.US, "global analytics getter parameter '%s' already exist", key);
            throw new AssertionError(msg);
        }
    }

    static public void addContextParameterBoolean(@Size(min = 1L,max = 40L) String key, boolean value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_context.put(key, value);
    }

    static public void addContextParameterString(@Size(min = 1L,max = 40L) String key, @Size(min = 1L,max = 100L) String value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_context.put(key, value);
    }

    static public void addContextParameterLong(@Size(min = 1L,max = 40L) String key, long value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_context.put(key, value);
    }

    static public void addContextParameterDouble(@Size(min = 1L,max = 40L) String key, double value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_context.put(key, value);
    }

    static public void addContextGetterParameterString(@Size(min = 1L,max = 40L) String key, MengineAnalyticsGetter<String> value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_getter.put(key, value);
    }

    static public void addContextGetterParameterLong(@Size(min = 1L,max = 40L) String key, MengineAnalyticsGetter<Long> value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_getter.put(key, value);
    }

    static public void addContextGetterParameterDouble(@Size(min = 1L,max = 40L) String key, MengineAnalyticsGetter<Double> value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_getter.put(key, value);
    }

    static private void collapseGetter(Map<String, Object> parameters) {
        for (Map.Entry<String, Object> entry : m_getter.entrySet()) {
            String key = entry.getKey();
            MengineAnalyticsGetter getter = (MengineAnalyticsGetter)entry.getValue();

            Object value = getter.get();

            parameters.put(key, value);
        }
    }

    static public MengineAnalyticsEventBuilder buildEvent(@Size(min = 1L,max = 40L) String name) {
        Map<String, Object> parameters = new HashMap<>();

        MengineAnalytics.collapseGetter(parameters);

        MengineAnalyticsEventBuilder eventBuilder = new MengineAnalyticsEventBuilder(MengineAnalytics.m_application, MengineAnalytics.m_context, parameters, name);

        return eventBuilder;
    }
}