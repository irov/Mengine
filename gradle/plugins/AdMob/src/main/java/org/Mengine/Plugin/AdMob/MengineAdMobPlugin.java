package org.Mengine.Plugin.AdMob;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineAdMobPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "MengineAdMob";

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        // Empty
    }
}
