package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineFragmentAnalytics extends MengineFragment<MengineListenerAnalytics> {
    static public MengineFragmentAnalytics INSTANCE = null;

    static protected String m_screenType = "unknown";
    static protected String m_screenName = "unknown";

    static public String getScreenType() {
        return m_screenType;
    }

    static public String getScreenName() {
        return m_screenName;
    }

    MengineFragmentAnalytics() {
        super(MengineListenerAnalytics.class);

        INSTANCE = this;
    }

    public void analyticsEvent(@NonNull MengineParamAnalyticsEvent param) {
        this.propagate(MengineListenerAnalytics::onMengineAnalyticsEvent, param);
    }

    public void analyticsScreenView(@NonNull String screenType, @NonNull String screenName) {
        m_screenType = screenType;
        m_screenName = screenName;

        MengineApplication.INSTANCE.setState("current.screen", screenType + "." + screenName);

        this.propagate(MengineListenerAnalytics::onMengineAnalyticsScreenView, screenType, screenName);
    }

    public void analyticsFlush() {
        this.propagate(MengineListenerAnalytics::onMengineAnalyticsFlush);
    }
}
