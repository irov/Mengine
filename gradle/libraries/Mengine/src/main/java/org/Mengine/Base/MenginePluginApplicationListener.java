package org.Mengine.Base;

import android.content.Context;
import android.content.res.Configuration;

public interface MenginePluginApplicationListener {
    void onAppCreate(MengineApplication application);
    void onAppTerminate(MengineApplication application);
    void onAppAttachBaseContext(MengineApplication application, Context base);
    void onAppConfigurationChanged(MengineApplication application, Configuration newConfig);
}
