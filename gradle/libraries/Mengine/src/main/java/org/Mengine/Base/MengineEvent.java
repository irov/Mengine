package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineEvent {
    public final static MengineEvent EVENT_GDPR_PASS = new MengineEvent("EVENT_GDPR_PASS");
    public final static MengineEvent EVENT_ADVERTISING_ID = new MengineEvent("EVENT_ADVERTISING_ID");
    public final static MengineEvent EVENT_SESSION_ID = new MengineEvent("EVENT_SESSION_ID");
    public final static MengineEvent EVENT_REMOTE_CONFIG_FETCH = new MengineEvent("EVENT_REMOTE_CONFIG_FETCH");

    private final String m_name;

    MengineEvent(String name) {
        m_name = name;
    }

    public String getName() {
        return m_name;
    }

    @NonNull
    public String toString() {
        return m_name;
    }
}