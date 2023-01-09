package org.Mengine.Plugin.AppLovin;

import com.applovin.mediation.MaxAd;

import org.Mengine.Base.MengineActivity;

public interface MengineAppLovinAnalyticsInterface {
    boolean initializeAnalytics(MengineActivity activity);
    void finalizeAnalytics();

    void eventRevenuePaid(MaxAd ad);
}
