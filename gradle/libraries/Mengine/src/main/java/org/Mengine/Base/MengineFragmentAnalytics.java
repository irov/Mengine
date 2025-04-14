package org.Mengine.Base;

import java.util.Map;

public class MengineFragmentAnalytics extends MengineFragment<MengineListenerAnalytics> {
    static public MengineFragmentAnalytics INSTANCE = null;

    MengineFragmentAnalytics() {
        super(MengineListenerAnalytics.class);

        INSTANCE = this;
    }

    public void analyticsEvent(String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        this.propagate(MengineListenerAnalytics::onMengineAnalyticsEvent, eventName, timestamp, bases, parameters);
    }

    public void analyticsScreenView(String screenType, String screenName) {
        MengineApplication.INSTANCE.setState("current.screen", screenType + "." + screenName);

        this.propagate(MengineListenerAnalytics::onMengineAnalyticsScreenView, screenType, screenName);
    }

    public void analyticsFlush() {
        this.propagate(MengineListenerAnalytics::onMengineAnalyticsFlush);
    }
}
