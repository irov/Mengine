package org.Mengine.Plugin.AdMob.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineFactorable;

public interface MengineAdMobAdInterface extends MengineFactorable {
    String getAdUnitId();

    void onActivityCreate(@NonNull MengineActivity activity);
    void onActivityDestroy(@NonNull MengineActivity activity);

    void loadAd();
}

