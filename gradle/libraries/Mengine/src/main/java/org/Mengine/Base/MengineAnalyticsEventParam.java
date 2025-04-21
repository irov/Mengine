package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.Map;

public class MengineAnalyticsEventParam {
    public MengineAnalyticsEventParam(@NonNull MengineAnalyticsEventCategory category, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        this.ANALYTICS_CATEGORY = category;
        this.ANALYTICS_NAME = eventName;
        this.ANALYTICS_TIMESTAMP = timestamp;
        this.ANALYTICS_BASES = bases;
        this.ANALYTICS_PARAMETERS = parameters;
    }

    public final @NonNull MengineAnalyticsEventCategory ANALYTICS_CATEGORY;
    public final String ANALYTICS_NAME;
    public final long ANALYTICS_TIMESTAMP;
    public final Map<String, Object> ANALYTICS_BASES;
    public final Map<String, Object> ANALYTICS_PARAMETERS;
}