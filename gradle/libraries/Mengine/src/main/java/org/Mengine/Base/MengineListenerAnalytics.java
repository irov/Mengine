package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerAnalytics extends MengineServiceInterface {
    default void onMengineAnalyticsEvent(@NonNull MengineApplication application, @NonNull MengineAnalyticsEventParam param) {
        //Empty
    }

    default void onMengineAnalyticsScreenView(@NonNull MengineApplication application, String screenType, String screenName) {
        //Empty
    }

    default void onMengineAnalyticsFlush(@NonNull MengineApplication application) {
        //Empty
    }
}
