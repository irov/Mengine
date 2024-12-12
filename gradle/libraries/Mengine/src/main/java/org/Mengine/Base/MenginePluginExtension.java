package org.Mengine.Base;

public class MenginePluginExtension {
    MengineApplication m_application;
    MengineActivity m_activity;
    MengineService m_plugin;

    public MengineApplication getMengineApplication() {
        return m_application;
    }
    public MengineActivity getMengineActivity() {
        return m_activity;
    }

    public MengineService getPlugin() {
        return m_plugin;
    }

    public boolean onPluginExtensionInitialize(MengineApplication application, MengineActivity activity, MengineService plugin)  throws MengineServiceInvalidInitializeException {
        m_application = application;
        m_activity = activity;
        m_plugin = plugin;

        return true;
    }

    public void onPluginExtensionFinalize(MengineActivity activity, MengineService plugin) {
        m_application = null;
        m_activity = null;
        m_plugin = null;
    }

    public void onPluginExtensionRun(MengineActivity activity, MengineService plugin) {
        //Empty
    }
}
