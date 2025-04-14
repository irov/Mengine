package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.Size;

import java.util.Map;

public class MengineAnalyticsEventBuilder {
    private final String m_name;
    private final Map<String, Object> m_bases;
    private final Map<String, Object> m_parameters;

    MengineAnalyticsEventBuilder(Map<String, Object> bases, Map<String, Object> parameters, @NonNull @Size(min = 1L,max = 40L) String name) {
        m_bases = bases;
        m_parameters = parameters;
        m_name = name;
    }

    private void assertBases(@NonNull String key) {
        if (BuildConfig.DEBUG == false) {
            return;
        }

        if (m_bases.containsKey(key)) {
            MengineUtils.throwAssertionError(MengineAnalytics.TAG, null
                , "event builder: %s parameter: %s already exist in context", m_name, key
            );
        }
    }

    private void assertParameters(@NonNull String key) {
        if (BuildConfig.DEBUG == false) {
            return;
        }

        if (m_parameters.containsKey(key)) {
            MengineUtils.throwAssertionError(MengineAnalytics.TAG, null
                , "event builder: %s parameter: %s already exist in parameters", m_name, key
            );
        }
    }

    private void assertJSON(@NonNull String key, @NonNull String json) {
        if (BuildConfig.DEBUG == false) {
            return;
        }

        if (MengineUtils.validateJSON(json) == false) {
            MengineUtils.throwAssertionError(MengineAnalytics.TAG, null
                , "event builder: %s parameter: %s invalid json: %s", m_name, key, json
            );
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

    public MengineAnalyticsEventBuilder addParameterException(@NonNull @Size(min = 1L,max = 40L) String key, @Nullable Exception e) {
        String message = e != null ? e.getMessage() : null;

        this.addParameterString(key, message != null ? message : "null");

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterThrowable(@NonNull @Size(min = 1L,max = 40L) String key, @NonNull Throwable e) {
        String message = e.getMessage();

        this.addParameterString(key, message != null ? message : "null");

        return this;
    }

    public MengineAnalyticsEventBuilder addParameterJSON(@NonNull @Size(min = 1L,max = 40L) String key, @NonNull @Size(min = 0L,max = 100L) String value) {
        this.assertJSON(key, value);

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

        MengineFragmentAnalytics.INSTANCE.analyticsFlush();

        return timestamp;
    }

    public long log() {
        long timestamp = MengineUtils.getTimestamp();

        MengineFragmentAnalytics.INSTANCE.analyticsEvent(m_name, timestamp, m_bases, m_parameters);

        return timestamp;
    }
}