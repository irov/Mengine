package org.Mengine.Plugin.AppLovin.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.json.JSONObject;

import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxAppOpenAd;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;
import com.applovin.mediation.nativeAds.MaxNativeAdLoader;

import java.util.Map;

public interface MengineAppLovinMediationInterface {
    void onAppCreate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin);
    void onAppTerminate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin);

    void onMengineRemoteConfigFetch(@NonNull MengineApplication application, @NonNull Map<String, JSONObject> configs);

    void loadMediatorBanner(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxAdView adView, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
    void loadMediatorInterstitial(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxInterstitialAd interstitialAd, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
    void loadMediatorRewarded(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxRewardedAd rewardedAd, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
    void loadMediatorMREC(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxAdView adView, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
    void loadMediatorNative(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxNativeAdLoader adLoader, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
    void loadMediatorAppOpen(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxAppOpenAd appOpenAd, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException;
}