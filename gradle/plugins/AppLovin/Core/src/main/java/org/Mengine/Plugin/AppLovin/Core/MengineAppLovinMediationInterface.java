package org.Mengine.Plugin.AppLovin.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;

public interface MengineAppLovinMediationInterface {
    void onAppCreate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin);
    void onAppTerminate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin);

    void onActivityCreate(@NonNull MengineActivity activity) throws MengineServiceInvalidInitializeException;
    void onActivityDestroy(@NonNull MengineActivity activity);

    void initializeMediatorBanner(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxAdView adView, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
    void loadMediatorInterstitial(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxInterstitialAd interstitialAd, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
    void loadMediatorRewarded(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxRewardedAd rewardedAd, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
}