package org.Mengine.Plugin.AppLovin.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;

public interface MengineAppLovinRewardedAdInterface extends MengineAppLovinAdInterface {
    boolean canOfferRewarded(String placement);
    boolean canYouShowRewarded(String placement);
    boolean showRewarded(@NonNull MengineActivity activity, String placement);
    boolean isShowingRewarded();
}
