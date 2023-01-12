package org.Mengine.Base;

import java.util.Vector;

public class MengineSemaphore {
    protected boolean m_activate;
    protected Vector<MengineCallbackInterface> m_listeners;

    MengineSemaphore(boolean _activate) {
        m_activate = _activate;

        if (m_activate == false) {
            m_listeners = new Vector<>();
        }
    }

    public void activate() {
        if  (m_activate == true) {
            return;
        }

        m_activate = true;

        for (MengineCallbackInterface cb : m_listeners) {
            cb.callback(this);
        }

        m_listeners = null;
    }

    public boolean addListener(MengineCallbackInterface cb) {
        if (m_activate == true) {
            cb.callback(this);

            return false;
        }

        m_listeners.add(cb);

        return true;
    }
}
