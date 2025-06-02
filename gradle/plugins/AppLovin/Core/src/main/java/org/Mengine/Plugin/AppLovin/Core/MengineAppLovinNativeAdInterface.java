package org.Mengine.Plugin.AppLovin.Core;

public interface MengineAppLovinNativeAdInterface extends MengineAppLovinAdInterface {
    int getLeftMarginPx();
    int getTopMarginPx();

    int getWidthPx();
    int getHeightPx();

    boolean canYouShow();

    void show();
    void hide();
}
