package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public interface MengineAppLovinNonetBannersInterface {
    void onAppCreate(MengineApplication application, MengineAppLovinPlugin plugin) throws MenginePluginInvalidInitializeException;
    void onAppTerminate(MengineApplication application, MengineAppLovinPlugin plugin);

    void onCreate(MengineActivity activity) throws MenginePluginInvalidInitializeException;
    void onDestroy();

    void show();
    void hide();
}