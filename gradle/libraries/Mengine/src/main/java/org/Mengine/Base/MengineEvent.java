package org.Mengine.Base;

public class MengineEvent {
    public final static MengineEvent EVENT_GDPR_PASS = new MengineEvent("GDPR");
    public final static MengineEvent EVENT_PUSH_TOKEN = new MengineEvent("PushToken");
    public final static MengineEvent EVENT_ADVERTISING_ID = new MengineEvent("AdvertisingId");
    public final static MengineEvent EVENT_SESSION_ID = new MengineEvent("SessionId");
    public final static MengineEvent EVENT_REMOTE_CONFIG_FETCH = new MengineEvent("RemoteConfigFetch");

    private String m_name;

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