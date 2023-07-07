package org.Mengine.Base;

import java.util.Map;

public interface MenginePluginAnalyticsListener extends MenginePluginInterface {
    int EAET_CUSTOM = MengineAnalytics.EAET_CUSTOM;
    int EAET_EARN_VIRTUAL_CURRENCY = MengineAnalytics.EAET_EARN_VIRTUAL_CURRENCY;
    int EAET_SPEND_VIRTUAL_CURRENCY = MengineAnalytics.EAET_SPEND_VIRTUAL_CURRENCY;
    int EAET_UNLOCK_ACHIEVEMENT = MengineAnalytics.EAET_UNLOCK_ACHIEVEMENT;
    int EAET_LEVEL_UP = MengineAnalytics.EAET_LEVEL_UP;
    int EAET_LEVEL_START = MengineAnalytics.EAET_LEVEL_START;
    int EAET_LEVEL_END = MengineAnalytics.EAET_LEVEL_END;
    int EAET_SELECT_ITEM = MengineAnalytics.EAET_SELECT_ITEM;
    int EAET_TUTORIAL_BEGIN = MengineAnalytics.EAET_TUTORIAL_BEGIN;
    int EAET_TUTORIAL_COMPLETE = MengineAnalytics.EAET_TUTORIAL_COMPLETE;
    int EAET_SCREEN_VIEW = MengineAnalytics.EAET_SCREEN_VIEW;

    void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> context, Map<String, Object> parameters);
}
