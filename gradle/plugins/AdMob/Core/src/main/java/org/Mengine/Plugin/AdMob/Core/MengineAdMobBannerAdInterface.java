package org.Mengine.Plugin.AdMob.Core;

public interface MengineAdMobBannerAdInterface extends MengineAdMobAdInterface {
    int getWidthPx();
    int getHeightPx();

    boolean canYouShow();

    void show();
    void hide();
}

