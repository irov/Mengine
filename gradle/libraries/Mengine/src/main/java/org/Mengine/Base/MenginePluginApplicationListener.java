package org.Mengine.Base;

import android.content.Context;
import android.content.res.Configuration;

import androidx.annotation.NonNull;

public interface MenginePluginApplicationListener extends MenginePluginInterface {
    default void onAppInit(MengineApplication application, boolean isMainProcess) throws MenginePluginInvalidInitializeException {
        //Empty
    }
    
    default void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        //Empty
    }

    default void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        //Empty
    }

    default void onAppTerminate(MengineApplication application) {
        //Empty
    }

    default void onAppAttachBaseContext(MengineApplication application, Context base) {
        //Empty
    }

    default void onAppConfigurationChanged(MengineApplication application, @NonNull Configuration newConfig) {
        //Empty
    }

    default void onAppEvent(MengineApplication application, MengineEvent event, Object ... args) {
        //Empty
    }

    default void onAppState(MengineApplication application, String name, Object value) {
        //Empty
    }
}
