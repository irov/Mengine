package org.Mengine.Plugin.AppLovin;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;

public interface MengineAppLovinMediationInterface {
    void initializeMediator(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin);
    void finalizeMediator(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin);

    void initializeMediatorBanner(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin, @NonNull MaxAdView adView, MengineAppLovinMediationLoadAdCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
    void loadMediatorInterstitial(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin, @NonNull MaxInterstitialAd interstitialAd, MengineAppLovinMediationLoadAdCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
    void loadMediatorRewarded(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin, @NonNull MaxRewardedAd rewardedAd, MengineAppLovinMediationLoadAdCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
}