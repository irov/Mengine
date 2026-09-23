package org.Mengine.Base;

public class MengineSemaphore {
    private volatile boolean m_activated;
    private Object m_value;

    MengineSemaphore(boolean activated, Object value) {
        m_value = value;
        m_activated = activated;
    }

    public boolean isActivated() {
        return m_activated;
    }

    public Object getValue() {
        return m_value;
    }

    public void activate(Object value) {
        m_value = value;
        m_activated = true;
    }
}