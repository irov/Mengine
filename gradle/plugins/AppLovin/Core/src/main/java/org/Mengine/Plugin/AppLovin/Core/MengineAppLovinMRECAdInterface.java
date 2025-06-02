package org.Mengine.Plugin.AppLovin.Core;

public interface MengineAppLovinMRECAdInterface extends MengineAppLovinAdInterface {
    int getLeftMarginPx();
    int getTopMarginPx();

    int getWidthPx();
    int getHeightPx();

    boolean canYouShow();

    void show(int leftMargin, int topMargin);
    void hide();
}