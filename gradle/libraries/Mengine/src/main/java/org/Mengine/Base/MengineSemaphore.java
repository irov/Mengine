package org.Mengine.Base;

import java.util.ArrayList;

public class MengineSemaphore {
    protected boolean m_activate;
    protected ArrayList<MengineFunctorVoid> m_listeners;

    MengineSemaphore(boolean _activate) {
        m_activate = _activate;

        if (m_activate == false) {
            m_listeners = new ArrayList<>();
        }
    }

    public void activate() {
        if  (m_activate == true) {
            return;
        }

        m_activate = true;

        for (MengineFunctorVoid cb : m_listeners) {
            cb.call();
        }

        m_listeners = null;
    }

    public boolean addListener(MengineFunctorVoid cb) {
        if (m_activate == true) {
            cb.call();

            return false;
        }

        m_listeners.add(cb);

        return true;
    }
}
