package org.Mengine.Base;

import android.content.Context;
import android.content.res.Configuration;

import androidx.annotation.NonNull;

public interface MenginePluginApplicationListener extends MenginePluginInterface {
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
}
