package org.Mengine.Base;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

public class MenginePluginExtension {
    MengineActivity m_activity;
    MenginePlugin m_plugin;

    public MengineActivity getActivity() {
        return m_activity;
    }

    public MenginePlugin getPlugin() {
        return m_plugin;
    }

    public boolean onPluginExtensionInitialize(MengineActivity activity, MenginePlugin plugin) {
        m_activity = activity;
        m_plugin = plugin;

        return true;
    }

    public void onPluginExtensionFinalize(MengineActivity activity, MenginePlugin plugin) {
        m_activity = null;
        m_plugin = null;
    }

    public void onPluginExtensionRun(MengineActivity activity, MenginePlugin plugin) {
        //Empty
    }
}
