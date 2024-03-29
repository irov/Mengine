package org.Mengine.Base;

public class MengineEvent {
    public final static MengineEvent EVENT_GDPR_PASS = new MengineEvent("EVENT_GDPR_PASS");
    public final static MengineEvent EVENT_PUSH_TOKEN = new MengineEvent("EVENT_PUSH_TOKEN");
    public final static MengineEvent EVENT_ADVERTISING_ID = new MengineEvent("EVENT_ADVERTISING_ID");
    public final static MengineEvent EVENT_SESSION_ID = new MengineEvent("EVENT_SESSION_ID");
    public final static MengineEvent EVENT_REMOTE_CONFIG_FETCH = new MengineEvent("EVENT_REMOTE_CONFIG_FETCH");
    public final static MengineEvent EVENT_TRANSPARENCY_CONSENT = new MengineEvent("EVENT_TRANSPARENCY_CONSENT");

    private final String m_name;

    MengineEvent(String name) {
        m_name = name;
    }

    public String getName() {
        return m_name;
    }
    public String toString() {
        return m_name;
    }
}