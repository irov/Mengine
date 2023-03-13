package org.Mengine.Base;

import android.content.Context;
import android.content.res.Configuration;

public interface MenginePluginApplicationListener {
    void onAppCreate(MengineApplication application);

    default void onAppTerminate(MengineApplication application) {
        //Empty
    }

    default void onAppAttachBaseContext(MengineApplication application, Context base) {
        //Empty
    }

    default void onAppConfigurationChanged(MengineApplication application, Configuration newConfig) {
        //Empty
    }
}
