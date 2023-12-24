package org.Mengine.Base;

public class MengineChainActivityNotFoundException extends Exception {
    protected String m_className;

    public MengineChainActivityNotFoundException(String className, String message) {
        super(message);

        this.m_className = className;
    }

    public String getClassName() {
        return this.m_className;
    }
}