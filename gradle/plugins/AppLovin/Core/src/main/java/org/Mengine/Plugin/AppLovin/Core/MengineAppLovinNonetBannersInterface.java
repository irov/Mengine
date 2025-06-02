package org.Mengine.Plugin.AppLovin.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

public interface MengineAppLovinNonetBannersInterface {
    void onAppCreate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin) throws MengineServiceInvalidInitializeException;
    void onAppTerminate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin);

    void onActivityCreate(@NonNull MengineActivity activity) throws MengineServiceInvalidInitializeException;
    void onActivityDestroy(@NonNull MengineActivity activity);

    void show();
    void hide();
}