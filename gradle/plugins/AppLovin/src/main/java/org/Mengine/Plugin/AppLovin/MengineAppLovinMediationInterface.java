package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;

import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;

public interface MengineAppLovinMediationInterface {
    void initializeMediator(MengineActivity activity);
    void initializeMediatorBanner(MengineActivity activity, MaxAdView adView);
    void loadMediatorInterstitial(MengineActivity activity, MaxInterstitialAd interstitialAd);
    void loadMediatorRewarded(MengineActivity activity, MaxRewardedAd rewardedAd);
}