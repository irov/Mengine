package org.Mengine.Base;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

public class MenginePluginExtension {
    MengineApplication m_application;
    MengineActivity m_activity;
    MenginePlugin m_plugin;

    public MengineApplication getMengineApplication() {
        return m_application;
    }
    public MengineActivity getMengineActivity() {
        return m_activity;
    }

    public MenginePlugin getPlugin() {
        return m_plugin;
    }

    public boolean onPluginExtensionInitialize(MengineApplication application, MengineActivity activity, MenginePlugin plugin)  throws MenginePluginInvalidInitializeException {
        m_application = application;
        m_activity = activity;
        m_plugin = plugin;

        return true;
    }

    public void onPluginExtensionFinalize(MengineActivity activity, MenginePlugin plugin) {
        m_application = null;
        m_activity = null;
        m_plugin = null;
    }

    public void onPluginExtensionRun(MengineActivity activity, MenginePlugin plugin) {
        //Empty
    }
}
