package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginProxyActivity;

public class MengineAppLovinProxyActivity extends MenginePluginProxyActivity {
    @Override
    public MenginePlugin getPlugin(MengineApplication application) {
        MenginePlugin plugin = application.getPlugin(MengineAppLovinPlugin.class);

        return plugin;
    }
}
