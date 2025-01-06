package org.Mengine.Plugin.AppLovin;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineApplication;

import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;

public interface MengineAppLovinMediationInterface {
    void initializeMediator(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin);
    void finalizeMediator(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin);

    void initializeMediatorBanner(@NonNull MengineApplication application, MaxAdView adView, MengineAppLovinMediationLoadAdCallback loadAdCallback);
    void loadMediatorInterstitial(@NonNull MengineApplication application, MaxInterstitialAd interstitialAd, MengineAppLovinMediationLoadAdCallback loadAdCallback);
    void loadMediatorRewarded(@NonNull MengineApplication application, MaxRewardedAd rewardedAd, MengineAppLovinMediationLoadAdCallback loadAdCallback);
}