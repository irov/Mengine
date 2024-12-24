package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.ArrayList;

public class MengineSemaphore {
    private boolean m_activated;
    private ArrayList<MengineSemaphoreListener> m_listeners = new ArrayList<>();

    MengineSemaphore(boolean _activated) {
        m_activated = _activated;
    }

    public boolean isActivated() {
        return m_activated;
    }

    public ArrayList<MengineSemaphoreListener> activate() {
        m_activated = true;

        ArrayList<MengineSemaphoreListener> listeners = m_listeners;
        m_listeners = null;

        return listeners;
    }

    public void addListener(@NonNull MengineSemaphoreListener _listener) {
        m_listeners.add(_listener);
    }
}