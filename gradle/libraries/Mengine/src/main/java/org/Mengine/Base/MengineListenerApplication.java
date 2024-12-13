package org.Mengine.Base;

import android.content.Context;
import android.content.res.Configuration;

import androidx.annotation.NonNull;

public interface MengineListenerApplication extends MengineServiceInterface {
    default void onAppInit(@NonNull MengineApplication application, boolean isMainProcess) throws MengineServiceInvalidInitializeException {
        //Empty
    }
    
    default void onAppPrepare(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        //Empty
    }

    default void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
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

    default void onAppEvent(@NonNull MengineApplication application, MengineEvent event, Object ... args) {
        //Empty
    }

    default void onAppState(@NonNull MengineApplication application, String name, Object value) {
        //Empty
    }
}
