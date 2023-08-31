package org.Mengine.Base;

public class MengineEvent {
    static public final MengineEvent EVENT_GDPR_PASS = new MengineEvent("GDPR");
    static public final MengineEvent EVENT_PUSH_TOKEN = new MengineEvent("PushToken");
    static public final MengineEvent EVENT_ADVERTISING_ID = new MengineEvent("AdvertisingId");
    static public final MengineEvent EVENT_SESSION_ID = new MengineEvent("SessionId");
    static public final MengineEvent EVENT_REMOTE_CONFIG_FETCH = new MengineEvent("RemoteConfigFetch");

    private String m_name;

    MengineEvent(String name) {
        m_name = name;
    }

    public String getName() {
        return m_name;
    }
}