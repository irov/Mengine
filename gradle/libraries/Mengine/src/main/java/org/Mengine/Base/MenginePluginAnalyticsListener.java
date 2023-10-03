package org.Mengine.Base;

import java.util.Map;

public interface MenginePluginAnalyticsListener extends MenginePluginInterface {
    void onMengineAnalyticsEvent(MengineApplication application, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters);
    void onMengineAnalyticsScreenView(MengineApplication application, String screenType, String screenName);

    void onMengineAnalyticsFlush(MengineApplication application);
}
