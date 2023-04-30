package org.Mengine.Base;

public class MenginePluginInvalidInitializeException extends Exception {
    private String m_pluginName;

    public MenginePluginInvalidInitializeException(String pluginName) {
        m_pluginName = pluginName;
    }

    public String getPluginName() {
        return m_pluginName;
    }
}