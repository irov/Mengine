package org.Mengine.Base;

public class MengineChainActivityNotFoundException extends Exception {
    protected final String m_className;

    public MengineChainActivityNotFoundException(String className, String message) {
        super(message);

        m_className = className;
    }

    public String getClassName() {
        return m_className;
    }
}