package org.Mengine.Plugin.AdMob.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;

public interface MengineAdMobAdInterface {
    String getAdUnitId();

    void onActivityCreate(@NonNull MengineActivity activity);
    void onActivityDestroy(@NonNull MengineActivity activity);

    void loadAd();
}

