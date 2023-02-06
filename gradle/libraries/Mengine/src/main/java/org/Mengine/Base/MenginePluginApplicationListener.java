package org.Mengine.Base;

import android.content.Context;
import android.content.res.Configuration;

public interface MenginePluginApplicationListener {
    public void onAppCreate(MengineApplication application);
    public void onAppTerminate(MengineApplication application);
    public void onAppAttachBaseContext(MengineApplication application, Context base);
    public void onAppConfigurationChanged(MengineApplication application, Configuration newConfig);
}
