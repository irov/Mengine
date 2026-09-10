package org.Mengine.Plugin.AdMob.Core;

import android.view.View;

import androidx.annotation.Nullable;

public interface MengineAdMobBannerAdInterface extends MengineAdMobAdInterface {
    int getWidthPx();
    int getHeightPx();

    @Nullable View getView();
    boolean isLoaded();

    boolean canYouShow();

    void show();
    void hide();
}

