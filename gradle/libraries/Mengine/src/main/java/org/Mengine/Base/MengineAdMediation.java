package org.Mengine.Base;

public class MengineAdMediation {
    public final static MengineAdMediation ADMEDIATION_APPLOVINMAX = new MengineAdMediation("APPLOVINMAX");

    private String m_name;

    MengineAdMediation(String name) {
        m_name = name;
    }

    public String getName() {
        return m_name;
    }
    public String toString() {
        return m_name;
    }
}