package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;

public interface MengineAppLovinNonetBannersInterface {
    void initializeNonetBanners(MengineActivity activity, MengineAppLovinPlugin plugin);
    void finalizeNonetBanners(MengineActivity activity, MengineAppLovinPlugin plugin);

    void show();
    void hide();
}