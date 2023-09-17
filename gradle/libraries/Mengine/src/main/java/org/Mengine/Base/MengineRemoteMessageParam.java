package org.Mengine.Base;

public class MengineRemoteMessageParam {
    public final static MengineRemoteMessageParam REMOTEMESSAGE_ID = new MengineRemoteMessageParam("ID");
    public final static MengineRemoteMessageParam REMOTEMESSAGE_FROM = new MengineRemoteMessageParam("FROM");
    public final static MengineRemoteMessageParam REMOTEMESSAGE_TO = new MengineRemoteMessageParam("TO");
    public final static MengineRemoteMessageParam REMOTEMESSAGE_COLLAPSE_KEY = new MengineRemoteMessageParam("COLLAPSE_KEY");
    public final static MengineRemoteMessageParam REMOTEMESSAGE_DATA = new MengineRemoteMessageParam("DATA");

    private String m_name;

    MengineRemoteMessageParam(String name) {
        m_name = name;
    }

    public String getName() {
        return m_name;
    }
    public String toString() {
        return m_name;
    }
}