package org.Mengine.Base;

public class MengineEvent {
    static public final MengineEvent EVENT_PUSH_TOKEN = new MengineEvent("PushToken");
    static public final MengineEvent EVENT_ADVERTISING_ID = new MengineEvent("AdvertisingId");

    private String m_name;

    MengineEvent(String name) {
        m_name = name;
    }

    public String getName() {
        return m_name;
    }
}