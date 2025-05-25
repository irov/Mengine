package org.Mengine.Base;

import android.content.Context;
import android.content.res.Configuration;

import androidx.annotation.NonNull;

import java.util.Map;

public interface MengineListenerApplication extends MengineServiceInterface {
    default void onAppPreInit(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        //Empty
    }

    default void onAppInit(@NonNull MengineApplication application, boolean isMainProcess) throws MengineServiceInvalidInitializeException {
        //Empty
    }

    default String onAppVersion(@NonNull MengineApplication application) {
        //Empty

        return null;
    }
    
    default void onAppPrepare(@NonNull MengineApplication application, @NonNull Map<String, String> pluginVersions) throws MengineServiceInvalidInitializeException {
        //Empty
    }

    default void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        //Empty
    }

    default void onAppPost(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        //Empty
    }

    default void onAppTerminate(@NonNull MengineApplication application) {
        //Empty
    }

    default void onAppAttachBaseContext(@NonNull MengineApplication application, Context base) {
        //Empty
    }

    default void onAppConfigurationChanged(@NonNull MengineApplication application, @NonNull Configuration newConfig) {
        //Empty
    }

    default void onAppLowMemory(@NonNull MengineApplication application) {
        //Empty
    }

    default void onAppTrimMemory(@NonNull MengineApplication application, int level) {
        //Empty
    }

    default void onAppState(@NonNull MengineApplication application, String name, Object value) {
        //Empty
    }
}
