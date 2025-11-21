package org.Mengine.Plugin.AdMob.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;

public interface MengineAdMobRewardedAdInterface extends MengineAdMobAdInterface {
    boolean canOfferRewarded(String placement);
    boolean canYouShowRewarded(String placement);
    boolean showRewarded(@NonNull MengineActivity activity, String placement);
    boolean isShowingRewarded();
}

