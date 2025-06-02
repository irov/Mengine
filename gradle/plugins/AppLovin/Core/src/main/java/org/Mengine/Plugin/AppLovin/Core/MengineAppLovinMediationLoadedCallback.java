package org.Mengine.Plugin.AppLovin.Core;

import androidx.annotation.NonNull;

public interface MengineAppLovinMediationLoadedCallback {
    void onAdLoaded(@NonNull MengineAppLovinMediationInterface mediation);
    void onAdLoadFailed(@NonNull MengineAppLovinMediationInterface mediation, @NonNull Exception exception);
}