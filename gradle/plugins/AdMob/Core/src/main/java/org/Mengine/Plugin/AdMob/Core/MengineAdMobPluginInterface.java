package org.Mengine.Plugin.AdMob.Core;

import androidx.annotation.Nullable;

import org.Mengine.Base.MengineServiceInterface;

public interface MengineAdMobPluginInterface extends MengineServiceInterface {
    @Nullable
    MengineAdMobBannerAdInterface getBannerAd();

    @Nullable
    MengineAdMobInterstitialAdInterface getInterstitialAd();

    @Nullable
    MengineAdMobRewardedAdInterface getRewardedAd();

    boolean showInterstitial(String placement);
    boolean showRewarded(String placement);
}

