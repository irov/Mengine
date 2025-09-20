package org.Mengine.Plugin.AppLovin.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;

public interface MengineAppLovinInterstitialAdInterface extends MengineAppLovinAdInterface {
    boolean canYouShowInterstitial(String placement);
    boolean showInterstitial(@NonNull MengineActivity activity, String placement);
    boolean isShowingInterstitial();
}
