package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.Map;

public interface MengineListenerAnalytics extends MengineServiceInterface {
    default void onMengineAnalyticsEvent(@NonNull MengineApplication application, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        //Empty
    }

    default void onMengineAnalyticsScreenView(@NonNull MengineApplication application, String screenType, String screenName) {
        //Empty
    }

    default void onMengineAnalyticsFlush(@NonNull MengineApplication application) {
        //Empty
    }
}
