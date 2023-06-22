package org.Mengine.Base;

import java.util.HashMap;
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

    private static MengineApplication m_application;
    private static Map<String, Object> m_context = new HashMap<>();
    private static Map<String, Object> m_getter = new HashMap<>();
    private static final Object m_lock = new Object();

    public static void setMengineApplication(MengineApplication application) {
        MengineAnalytics.m_application = application;
    }

    static private void assertContext(String key) {
        if (MengineAnalytics.m_context.containsKey(key)) {
            String msg = String.format("global analytics context parameter '%s' already exist", key);
            throw new AssertionError(msg);
        }
    }

    static private void assertGetter(String key) {
        if (MengineAnalytics.m_getter.containsKey(key)) {
            String msg = String.format("global analytics getter parameter '%s' already exist", key);
            throw new AssertionError(msg);
        }
    }

    static public void addContextParameterBoolean(String key, boolean value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_context.put(key, value);
    }

    static public void addContextParameterString(String key, String value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_context.put(key, value);
    }

    static public void addContextParameterLong(String key, long value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_context.put(key, value);
    }

    static public void addContextParameterDouble(String key, double value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_context.put(key, value);
    }

    static public void addContextGetterParameterString(String key, MengineAnalyticsGetter<String> value) {
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
        };
    }

    static public MengineAnalyticsEventBuilder buildEvent(String name) {
        Map<String, Object> parameters = new HashMap<>();

        MengineAnalytics.collapseGetter(parameters);

        MengineAnalyticsEventBuilder eventBuilder = new MengineAnalyticsEventBuilder(MengineAnalytics.m_application, MengineAnalytics.m_context, parameters, MengineAnalytics.m_lock, name);

        return eventBuilder;
    }
}