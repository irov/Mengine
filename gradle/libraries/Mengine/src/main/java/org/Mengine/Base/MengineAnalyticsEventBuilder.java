package org.Mengine.Base;

import java.util.Locale;
import java.util.Map;

public class MengineAnalyticsEventBuilder {
    private MengineApplication m_application;

    private String m_name;
    private Map<String, Object> m_context;
    private Map<String, Object> m_parameters;

    MengineAnalyticsEventBuilder(MengineApplication application, Map<String, Object> context, Map<String, Object> parameters, String name) {
        m_application = application;
        m_context = context;
        m_parameters = parameters;
        m_name = name;
    }

    private void assertContext(String key) {
        if (m_context.containsKey(key)) {
            String msg = String.format(Locale.US, "event builder '%s' parameter '%s' already exist in context", m_name, key);
            throw new AssertionError(msg);
        }
    }

    private void assertParameters(String key) {
        if (m_parameters.containsKey(key)) {
            String msg = String.format(Locale.US, "event builder '%s' parameter '%s' already exist in parameters", m_name, key);
            throw new AssertionError(msg);
        }
    }

    public MengineAnalyticsEventBuilder addParameterBoolean(String key, boolean value) {
        this.assertContext(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterString(String key, String value) {
        this.assertContext(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterLong(String key, long value) {
        this.assertContext(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterDouble(String key, double value) {
        this.assertContext(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public long log() {
        long log_timestamp = MengineUtils.getTimestamp();

        m_application.onMengineAnalyticsEvent(MengineAnalytics.EAET_CUSTOM, m_name, log_timestamp, m_context, m_parameters);

        return log_timestamp;
    }
}