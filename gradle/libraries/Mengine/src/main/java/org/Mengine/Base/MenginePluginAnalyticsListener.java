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

    String EA_ADMEDIATION_APPLOVINMAX = MengineAnalytics.EA_ADMEDIATION_APPLOVINMAX;

    int EA_ADTYPE_BANNER = MengineAnalytics.EA_ADTYPE_BANNER;
    int EA_ADTYPE_MREC = MengineAnalytics.EA_ADTYPE_MREC;
    int EA_ADTYPE_LEADER = MengineAnalytics.EA_ADTYPE_LEADER;
    int EA_ADTYPE_INTERSTITIAL = MengineAnalytics.EA_ADTYPE_INTERSTITIAL;
    int EA_ADTYPE_APP_OPEN = MengineAnalytics.EA_ADTYPE_APP_OPEN;
    int EA_ADTYPE_REWARDED = MengineAnalytics.EA_ADTYPE_REWARDED;
    int EA_ADTYPE_REWARDED_INTERSTITIAL = MengineAnalytics.EA_ADTYPE_REWARDED_INTERSTITIAL;
    int EA_ADTYPE_NATIVE = MengineAnalytics.EA_ADTYPE_NATIVE;
    int EA_ADTYPE_CROSS_PROMO = MengineAnalytics.EA_ADTYPE_CROSS_PROMO;

    String EA_ADREVENUE_SOURCE = MengineAnalytics.EA_ADREVENUE_SOURCE;
    String EA_ADREVENUE_TYPE = MengineAnalytics.EA_ADREVENUE_TYPE;
    String EA_ADREVENUE_ADUNITID = MengineAnalytics.EA_ADREVENUE_ADUNITID;
    String EA_ADREVENUE_PLACEMENT = MengineAnalytics.EA_ADREVENUE_PLACEMENT;
    String EA_ADREVENUE_NETWORK = MengineAnalytics.EA_ADREVENUE_NETWORK;
    String EA_ADREVENUE_REVENUE_PRECISION = MengineAnalytics.EA_ADREVENUE_REVENUE_PRECISION;
    String EA_ADREVENUE_REVENUE_VALUE = MengineAnalytics.EA_ADREVENUE_REVENUE_VALUE;
    String EA_ADREVENUE_REVENUE_CURRENCY = MengineAnalytics.EA_ADREVENUE_REVENUE_CURRENCY;

    void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters);

    void onMengineAnalyticsFlush(MengineApplication application);

    void onMengineAnalyticsRevenuePaid(MengineApplication application, Map<String, Object> paid);
}
