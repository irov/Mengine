package org.Mengine.Base;

import java.util.Map;

public interface MenginePluginAnalyticsListener {
    int EAET_CUSTOM = MengineAnalytics.EAET_CUSTOM;
    int EAET_EARN_VIRTUAL_CURRENCY = MengineAnalytics.EAET_EARN_VIRTUAL_CURRENCY;
    int EAET_SPEND_VIRTUAL_CURRENCY = MengineAnalytics.EAET_SPEND_VIRTUAL_CURRENCY;
    int EAET_UNLOCK_ACHIEVEMENT = MengineAnalytics.EAET_UNLOCK_ACHIEVEMENT;

    void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> parameters);
}
