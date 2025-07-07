package org.Mengine.Plugin.AppLovin.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;

public interface MengineAppLovinAdInterface {
    String getAdUnitId();

    void onActivityCreate(@NonNull MengineActivity activity);
    void onActivityDestroy(@NonNull MengineActivity activity);

    void loadAd();
}
