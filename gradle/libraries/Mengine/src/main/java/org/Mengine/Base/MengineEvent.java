package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineEvent {
    public final static MengineEvent EVENT_GDPR_PASS = new MengineEvent("EVENT_GDPR_PASS");

    private final String m_name;

    MengineEvent(@NonNull String name) {
        m_name = name;
    }

    public String getName() {
        return m_name;
    }

    @Override
    @NonNull
    public String toString() {
        return m_name;
    }
}