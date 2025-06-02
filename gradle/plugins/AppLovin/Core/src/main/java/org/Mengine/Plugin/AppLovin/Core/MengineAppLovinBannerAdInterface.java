package org.Mengine.Plugin.AppLovin.Core;

public interface MengineAppLovinBannerAdInterface extends MengineAppLovinAdInterface {
    int getWidthPx();
    int getHeightPx();

    boolean canYouShow();

    void show();
    void hide();
}
