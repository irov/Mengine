package org.Mengine.Base;

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

    public final static String EA_ADMEDIATION_APPLOVINMAX = "AppLovinMax";

    public final static int EA_ADTYPE_BANNER = 0;
    public final static int EA_ADTYPE_MREC = 1;
    public final static int EA_ADTYPE_LEADER = 2;
    public final static int EA_ADTYPE_INTERSTITIAL = 3;
    public final static int EA_ADTYPE_APP_OPEN = 4;
    public final static int EA_ADTYPE_REWARDED = 5;
    public final static int EA_ADTYPE_REWARDED_INTERSTITIAL = 6;
    public final static int EA_ADTYPE_NATIVE = 7;
    public final static int EA_ADTYPE_CROSS_PROMO = 8;
    public final static int EA_ADTYPE_UNKNOWN = 8;

    public final static String EA_ADREVENUE_SOURCE = "SOURCE";
    public final static String EA_ADREVENUE_TYPE = "TYPE";
    public final static String EA_ADREVENUE_ADUNITID = "ADUNITID";
    public final static String EA_ADREVENUE_PLACEMENT = "PLACEMENT";
    public final static String EA_ADREVENUE_NETWORK = "NETWORK";
    public final static String EA_ADREVENUE_REVENUE_PRECISION = "REVENUE_PRECISION";
    public final static String EA_ADREVENUE_REVENUE_VALUE = "REVENUE_VALUE";
    public final static String EA_ADREVENUE_REVENUE_CURRENCY = "REVENUE_CURRENCY";

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

    static public void addContextGetterParameterLong(String key, MengineAnalyticsGetter<Long> value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_getter.put(key, value);
    }

    static public void addContextGetterParameterDouble(String key, MengineAnalyticsGetter<Double> value) {
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

    static public MengineAnalyticsEventBuilder buildEvent(String name) {
        Map<String, Object> parameters = new HashMap<>();

        MengineAnalytics.collapseGetter(parameters);

        MengineAnalyticsEventBuilder eventBuilder = new MengineAnalyticsEventBuilder(MengineAnalytics.m_application, MengineAnalytics.m_context, parameters, name);

        return eventBuilder;
    }
}