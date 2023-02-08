package org.Mengine.Base;

import java.util.Map;

public interface MenginePluginAnalyticsListener {
    static public int EAET_CUSTOM = 0;
    static public int EAET_EARN_VIRTUAL_CURRENCY = 1;
    static public int EAET_SPEND_VIRTUAL_CURRENCY = 2;
    static public int EAET_UNLOCK_ACHIEVEMENT = 3;

    public void onMengineAnalyticsEvent(MengineActivity activity, int eventType, String eventName, long timestamp, Map<String, Object> parameters);
}
