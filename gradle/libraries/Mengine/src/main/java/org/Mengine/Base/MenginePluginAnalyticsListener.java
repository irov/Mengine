package org.Mengine.Base;

import java.util.Map;

public interface MenginePluginAnalyticsListener {
    int EAET_CUSTOM = 0;
    int EAET_EARN_VIRTUAL_CURRENCY = 1;
    int EAET_SPEND_VIRTUAL_CURRENCY = 2;
    int EAET_UNLOCK_ACHIEVEMENT = 3;

    void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> parameters);
}
