package org.Mengine.Plugin.AppLovin;

import com.applovin.mediation.MaxAd;

import org.Mengine.Base.MengineActivity;

public interface MengineAppLovinAnalyticsInterface {
    public boolean initializeAnalytics(MengineActivity activity);
    public void finalizeAnalytics();

    public void eventRevenuePaid(MaxAd ad);
}
