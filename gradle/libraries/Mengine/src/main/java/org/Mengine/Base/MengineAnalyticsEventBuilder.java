package org.Mengine.Base;

import java.util.Locale;
import java.util.Map;

public class MengineAnalyticsEventBuilder {
    private final MengineApplication m_application;

    private final String m_name;
    private final Map<String, Object> m_bases;
    private final Map<String, Object> m_parameters;

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

    public MengineAnalyticsEventBuilder addParameterException(String key, Exception e) {
        this.assertBases(key);
        this.assertParameters(key);

        String message = e.getMessage();

        m_parameters.put(key, message);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterThrowable(String key, Throwable e) {
        this.assertBases(key);
        this.assertParameters(key);

        String message = e.getMessage();

        m_parameters.put(key, message);

        return this;
    }

    private void validateJSON(String key, String json) {
        if (json == null || json.isEmpty() == true || json.length() < 2 || json.charAt(0) != '{' || json.charAt(json.length() - 1) != '}') {
            String msg = String.format(Locale.US, "event builder '%s' parameter '%s' invalid json", m_name, key);
            throw new AssertionError(msg);
        }
    }

    public MengineAnalyticsEventBuilder addParameterJSON(String key, String value) {
        this.assertBases(key);
        this.assertParameters(key);
        this.validateJSON(key, value);

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

        m_application.onMengineAnalyticsEvent(m_name, timestamp, m_bases, m_parameters);

        return timestamp;
    }
}