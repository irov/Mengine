package org.Mengine.Base;

import java.util.Locale;
import java.util.Map;

public class MengineAnalyticsEventBuilder {
    private MengineApplication m_application;

    private String m_name;
    private Map<String, Object> m_bases;
    private Map<String, Object> m_parameters;

    MengineAnalyticsEventBuilder(MengineApplication application, Map<String, Object> bases, Map<String, Object> parameters, String name) {
        m_application = application;
        m_bases = bases;
        m_parameters = parameters;
        m_name = name;
    }

    private void assertBases(String key) {
        if (m_bases.containsKey(key)) {
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
        this.assertBases(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterString(String key, String value) {
        this.assertBases(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterJSON(String key, String value) {
        this.assertBases(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterLong(String key, long value) {
        this.assertBases(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterDouble(String key, double value) {
        this.assertBases(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public long logAndFlush() {
        long timestamp = this.log();

        m_application.onMengineAnalyticsFlush();

        return timestamp;
    }

    public long log() {
        long timestamp = MengineUtils.getTimestamp();

        m_application.onMengineAnalyticsEvent(MengineAnalytics.EAET_CUSTOM, m_name, timestamp, m_bases, m_parameters);

        return timestamp;
    }
}