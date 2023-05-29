package org.Mengine.Base;

import java.util.HashMap;
import java.util.Map;

public class MengineAnalyticsEventBuilder {
    private static native long AndroidAnalyticsService_logEventBuilder(String name, Map<String, Object> parameters);

    private MengineApplication m_application;

    private Object m_lock;
    private String m_name;
    private Map<String, Object> m_parameters = new HashMap<>();

    MengineAnalyticsEventBuilder(MengineApplication application, Object lock, String name) {
        m_application = application;
        m_lock = lock;
        m_name = name;
    }

    public MengineAnalyticsEventBuilder addParameterBoolean(String name, boolean value) {
        m_parameters.put(name, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterString(String name, String value) {
        m_parameters.put(name, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterLong(String name, long value) {
        m_parameters.put(name, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterDouble(String name, double value) {
        m_parameters.put(name, value);

        return this;
    }

    public long log() {
        long log_timestamp = MengineUtils.getTimestamp();

        synchronized (this.m_lock) {
            m_application.onMengineAnalyticsEvent(MengineAnalytics.EAET_CUSTOM, m_name, log_timestamp, m_parameters);
        }

        return log_timestamp;
    }
}