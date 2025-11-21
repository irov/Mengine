package org.Mengine.Plugin.AdMob.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;

public interface MengineAdMobInterstitialAdInterface extends MengineAdMobAdInterface {
    boolean canYouShowInterstitial(String placement);
    boolean showInterstitial(@NonNull MengineActivity activity, String placement);
    boolean isShowingInterstitial();
}

