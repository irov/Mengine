package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;

import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;

public interface MengineAppLovinMediationInterface {
    void initializeMediator(MengineActivity activity, MengineAppLovinPlugin plugin);
    void finalizeMediator(MengineActivity activity, MengineAppLovinPlugin plugin);

    void initializeMediatorBanner(MengineActivity activity, MengineAppLovinPlugin plugin, MaxAdView adView, MengineAppLovinMediationLoadAdCallback loadAdCallback);
    void loadMediatorInterstitial(MengineActivity activity, MengineAppLovinPlugin plugin, MaxInterstitialAd interstitialAd, MengineAppLovinMediationLoadAdCallback loadAdCallback);
    void loadMediatorRewarded(MengineActivity activity, MengineAppLovinPlugin plugin, MaxRewardedAd rewardedAd, MengineAppLovinMediationLoadAdCallback loadAdCallback);
}