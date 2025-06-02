package org.Mengine.Plugin.AppLovin.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;

public interface MengineAppLovinAdInterface {
    boolean isInitialized();

    void initialize(@NonNull MengineApplication application);
    void finalize(@NonNull MengineApplication application);

    String getAdUnitId();

    void onActivityCreate(@NonNull MengineActivity activity);
    void onActivityDestroy(@NonNull MengineActivity activity);

    void loadAd();
}
