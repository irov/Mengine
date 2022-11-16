package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;

import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;

public interface MengineAppLovinMediationInterface {
    public void initializeMediator(MengineActivity activity) throws Exception;
    public void initializeMediatorBanner(MengineActivity activity, MaxAdView adView) throws Exception;
    public void loadMediatorInterstitial(MengineActivity activity, MaxInterstitialAd interstitialAd) throws Exception;
    public void loadMediatorRewarded(MengineActivity activity, MaxRewardedAd rewardedAd) throws Exception;
}