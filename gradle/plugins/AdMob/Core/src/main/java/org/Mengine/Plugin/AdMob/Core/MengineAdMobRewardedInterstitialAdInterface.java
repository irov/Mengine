package org.Mengine.Plugin.AdMob.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;

public interface MengineAdMobRewardedInterstitialAdInterface extends MengineAdMobAdInterface {
    boolean canYouShowRewardedInterstitial(String placement);
    boolean showRewardedInterstitial(@NonNull MengineActivity activity, String placement);
    boolean isShowingRewardedInterstitial();
}
