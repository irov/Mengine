package org.Mengine.Base;

import java.util.Map;

public interface MengineAnalyticsListener {
    public void onMengineAnalyticsEvent(MengineActivity activity, String eventName, long timestamp, Map<String, Object> parameters);
}
