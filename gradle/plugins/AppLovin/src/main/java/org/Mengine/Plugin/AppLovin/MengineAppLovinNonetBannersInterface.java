package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public interface MengineAppLovinNonetBannersInterface {
    void initializeNonetBanners(MengineActivity activity, MengineAppLovinPlugin plugin) throws MenginePluginInvalidInitializeException;
    void finalizeNonetBanners(MengineActivity activity, MengineAppLovinPlugin plugin);

    void show();
    void hide();
}