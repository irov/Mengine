package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerAnalytics extends MengineServiceInterface {
    default void onMengineAnalyticsEvent(@NonNull MengineApplication application, @NonNull MengineParamAnalyticsEvent param) {
        //Empty
    }

    default void onMengineAnalyticsScreenView(@NonNull MengineApplication application, @NonNull String screenType, @NonNull String screenName) {
        //Empty
    }

    default void onMengineAnalyticsFlush(@NonNull MengineApplication application) {
        //Empty
    }
}
