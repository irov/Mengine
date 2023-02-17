package org.Mengine.Plugin.AppLovin;

import com.applovin.mediation.MaxAd;

public interface MengineAppLovinAnalyticsListener {
    void onEventRevenuePaid(MengineAppLovinPlugin plugin, MaxAd ad);
}
