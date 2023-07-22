package org.Mengine.Base;

public class MenginePluginInvalidInitializeException extends Exception {
    private final String m_pluginName;

    public MenginePluginInvalidInitializeException(String pluginName, String message) {
        super(message);

        m_pluginName = pluginName;
    }

    public String getPluginName() {
        return m_pluginName;
    }
}