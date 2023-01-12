package org.Mengine.Plugin.AppLovin;

import com.applovin.mediation.MaxAd;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

public interface MengineAppLovinAnalyticsInterface {
    boolean initializeAnalytics(MenginePlugin plugin, MengineActivity activity);
    void finalizeAnalytics(MenginePlugin plugin);

    void onEventRevenuePaid(MenginePlugin plugin, MaxAd ad);
}
