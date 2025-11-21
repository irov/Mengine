package org.Mengine.Plugin.AppLovin.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.json.JSONObject;

import java.util.Map;

public interface MengineAppLovinNonetBannersInterface {
    void onAppCreate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin) throws MengineServiceInvalidInitializeException;
    void onAppTerminate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin);

    void onMengineRemoteConfigFetch(@NonNull MengineApplication application, boolean updated, @NonNull Map<String, JSONObject> configs, @NonNull Map<String, Integer> ids);

    void onActivityCreate(@NonNull MengineActivity activity);
    void onActivityDestroy(@NonNull MengineActivity activity);

    void show();
    void hide();
}