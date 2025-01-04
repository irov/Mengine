package org.Mengine.Plugin.AppLovin;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

public interface MengineAppLovinAppOpenAdInterface {
    void initializeAppOpenAd(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin);
    void finalizeAppOpenAd(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin);
}
