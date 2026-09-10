package org.Mengine.Plugin.AppLovin.Core;

import android.view.View;

import androidx.annotation.Nullable;

public interface MengineAppLovinBannerAdInterface extends MengineAppLovinAdInterface {
    int getWidthPx();
    int getHeightPx();

    @Nullable View getView();
    boolean isLoaded();

    boolean canYouShow();

    void show();
    void hide();
}
