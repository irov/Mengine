package org.Mengine.Base;

import androidx.annotation.Size;

import java.util.HashMap;
import java.util.Map;

public class MengineAnalytics {
    public static final MengineTag TAG = MengineTag.of("MNGAnalytics");

    private static final Map<String, Object> m_bases = new HashMap<>();
    private static final Map<String, Object> m_getter = new HashMap<>();

    static private void assertContext(String key) {
        if (BuildConfig.DEBUG == false) {
            return;
        }

        if (MengineAnalytics.m_bases.containsKey(key) == true) {
            MengineUtils.throwAssertionError(MengineAnalytics.TAG, null
                , "global analytics context parameter: %s already exist", key
            );
        }
    }

    static private void assertGetter(String key) {
        if (BuildConfig.DEBUG == false) {
            return;
        }

        if (MengineAnalytics.m_getter.containsKey(key) == true) {
            MengineUtils.throwAssertionError(MengineAnalytics.TAG, null
                , "global analytics getter parameter: %s already exist", key
            );
        }
    }

    static public void addContextParameterBoolean(@Size(min = 1L,max = 40L) String key, boolean value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_bases.put(key, value);
    }

    static public void addContextParameterString(@Size(min = 1L,max = 40L) String key, @Size(min = 1L,max = 100L) String value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_bases.put(key, value);
    }

    static public void addContextParameterLong(@Size(min = 1L,max = 40L) String key, long value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_bases.put(key, value);
    }

    static public void addContextParameterDouble(@Size(min = 1L,max = 40L) String key, double value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_bases.put(key, value);
    }

    static public void addContextGetterParameterBoolean(@Size(min = 1L,max = 40L) String key, MengineAnalyticsGetter<Boolean> value) {
        MengineAnalytics.assertContext(key);
        MengineAnalytics.assertGetter(key);

        MengineAnalytics.m_getter.put(key, value);
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

    static public Map<String, Object> collapseContextParameters() {
        Map<String, Object> parameters = new HashMap<>(MengineAnalytics.m_bases);

        MengineAnalytics.collapseGetter(parameters);

        return parameters;
    }

    static private void collapseGetter(Map<? super String, Object> parameters) {
        for (Map.Entry<String, Object> entry : MengineAnalytics.m_getter.entrySet()) {
            String key = entry.getKey();
            MengineAnalyticsGetter getter = (MengineAnalyticsGetter)entry.getValue();

            Object value = getter.get();

            parameters.put(key, value);
        }
    }

    static public MengineAnalyticsEventBuilderInterface buildEvent(@Size(min = 1L,max = 40L) String name) {
        Map<String, Object> parameters = new HashMap<>();

        MengineAnalytics.collapseGetter(parameters);

        MengineAnalyticsEventBuilderInterface eventBuilder = new MengineAnalyticsEventBuilder(MengineAnalytics.m_bases, parameters, name);

        return eventBuilder;
    }

    static public MengineAnalyticsEventBuilderInterface buildEventDummy(@Size(min = 1L,max = 40L) String name) {
        MengineAnalyticsEventBuilderInterface eventBuilder = new MengineAnalyticsEventBuilderDummy(name);

        return eventBuilder;
    }
}