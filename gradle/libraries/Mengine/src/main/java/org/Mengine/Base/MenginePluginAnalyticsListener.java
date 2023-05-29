package org.Mengine.Base;

import java.util.Map;

public interface MenginePluginAnalyticsListener {
    public final static int EAET_CUSTOM = MengineAnalytics.EAET_CUSTOM;
    public final static int EAET_EARN_VIRTUAL_CURRENCY = MengineAnalytics.EAET_EARN_VIRTUAL_CURRENCY;
    public final static int EAET_SPEND_VIRTUAL_CURRENCY = MengineAnalytics.EAET_SPEND_VIRTUAL_CURRENCY;
    public final static int EAET_UNLOCK_ACHIEVEMENT = MengineAnalytics.EAET_UNLOCK_ACHIEVEMENT;

    void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> parameters);
}
