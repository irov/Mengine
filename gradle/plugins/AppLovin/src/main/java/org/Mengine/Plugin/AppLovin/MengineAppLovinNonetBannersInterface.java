package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

public interface MengineAppLovinNonetBannersInterface {
    void onAppCreate(MengineApplication application, MengineAppLovinPlugin plugin) throws MengineServiceInvalidInitializeException;
    void onAppTerminate(MengineApplication application, MengineAppLovinPlugin plugin);

    void onCreate(MengineActivity activity) throws MengineServiceInvalidInitializeException;
    void onDestroy();

    void show();
    void hide();
}