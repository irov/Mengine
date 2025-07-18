package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineFragmentAnalytics extends MengineFragment<MengineListenerAnalytics> {
    static public MengineFragmentAnalytics INSTANCE = null;

    MengineFragmentAnalytics() {
        super(MengineListenerAnalytics.class);

        INSTANCE = this;
    }

    public void analyticsEvent(@NonNull MengineParamAnalyticsEvent param) {
        this.propagate(MengineListenerAnalytics::onMengineAnalyticsEvent, param);
    }

    public void analyticsScreenView(@NonNull String screenType, @NonNull String screenName) {
        MengineApplication.INSTANCE.setState("current.screen", screenType + "." + screenName);

        this.propagate(MengineListenerAnalytics::onMengineAnalyticsScreenView, screenType, screenName);
    }

    public void analyticsFlush() {
        this.propagate(MengineListenerAnalytics::onMengineAnalyticsFlush);
    }
}
