package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineAdMediation {
    public final static MengineAdMediation ADMEDIATION_APPLOVINMAX = new MengineAdMediation("ADMEDIATION_APPLOVINMAX");

    private final String m_name;

    MengineAdMediation(@NonNull String name) {
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