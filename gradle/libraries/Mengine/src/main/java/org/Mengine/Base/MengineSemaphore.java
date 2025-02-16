package org.Mengine.Base;

public class MengineSemaphore {
    private boolean m_activated;

    MengineSemaphore(boolean _activated) {
        m_activated = _activated;
    }

    public boolean isActivated() {
        return m_activated;
    }

    public void activate() {
        m_activated = true;
    }
}