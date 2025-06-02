package org.Mengine.Plugin.AppLovin.Core;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineServiceInvalidInitializeException;

public interface MengineAppLovinMediationLoadInterface {
    void onAdLoad(@NonNull MengineAppLovinMediationInterface mediation, @NonNull MengineAppLovinMediationLoadedCallback callback) throws MengineServiceInvalidInitializeException;
}