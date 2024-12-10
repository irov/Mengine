package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import java.util.Locale;
import java.util.Map;

public class MengineAnalyticsEventBuilder {
    private final MengineApplication m_application;

    private final String m_name;
    private final Map<String, Object> m_bases;
    private final Map<String, Object> m_parameters;

    MengineAnalyticsEventBuilder(MengineApplication application, Map<String, Object> bases, Map<String, Object> parameters, @NonNull @Size(min = 1L,max = 40L) String name) {
        m_application = application;
        m_bases = bases;
        m_parameters = parameters;
        m_name = name;
    }

    private void assertBases(@NonNull String key) {
        if (m_bases.containsKey(key)) {
            String msg = String.format(Locale.US, "event builder '%s' parameter '%s' already exist in context", m_name, key);
            throw new AssertionError(msg);
        }
    }

    private void assertParameters(@NonNull String key) {
        if (m_parameters.containsKey(key)) {
            String msg = String.format(Locale.US, "event builder '%s' parameter '%s' already exist in parameters", m_name, key);
            throw new AssertionError(msg);
        }
    }

    public MengineAnalyticsEventBuilder addParameterBoolean(@NonNull @Size(min = 1L,max = 40L) String key, boolean value) {
        this.assertBases(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterString(@NonNull @Size(min = 1L,max = 40L) String key, @NonNull @Size(min = 0L,max = 100L) String value) {
        this.assertBases(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterException(@NonNull @Size(min = 1L,max = 40L) String key, @NonNull Exception e) {
        String message = e.getMessage();

        this.addParameterString(key, message != null ? message : "null");

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterThrowable(@NonNull @Size(min = 1L,max = 40L) String key, @NonNull Throwable e) {
        String message = e.getMessage();

        this.addParameterString(key, message != null ? message : "null");

        return this;
    }

    private void validateJSON(@NonNull String key, @NonNull String json) {
        if (json == null || json.isEmpty() == true || json.length() < 2 || json.charAt(0) != '{' || json.charAt(json.length() - 1) != '}') {
            String msg = String.format(Locale.US, "event builder '%s' parameter '%s' invalid json", m_name, key);
            throw new AssertionError(msg);
        }
    }

    public MengineAnalyticsEventBuilder addParameterJSON(@NonNull @Size(min = 1L,max = 40L) String key, @NonNull @Size(min = 0L,max = 100L) String value) {
        this.validateJSON(key, value);

        this.addParameterString(key, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterLong(@NonNull @Size(min = 1L,max = 40L) String key, long value) {
        this.assertBases(key);
        this.assertParameters(key);

        m_parameters.put(key, value);

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterDouble(@NonNull @Size(min = 1L,max = 40L) String key, double value) {
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