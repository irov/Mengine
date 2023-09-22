package org.Mengine.Base;

public class MengineRemoteMessageParam {
    public final static MengineRemoteMessageParam REMOTEMESSAGE_ID = new MengineRemoteMessageParam("REMOTEMESSAGE_ID");
    public final static MengineRemoteMessageParam REMOTEMESSAGE_FROM = new MengineRemoteMessageParam("REMOTEMESSAGE_FROM");
    public final static MengineRemoteMessageParam REMOTEMESSAGE_TO = new MengineRemoteMessageParam("REMOTEMESSAGE_TO");
    public final static MengineRemoteMessageParam REMOTEMESSAGE_COLLAPSE_KEY = new MengineRemoteMessageParam("REMOTEMESSAGE_COLLAPSE_KEY");
    public final static MengineRemoteMessageParam REMOTEMESSAGE_DATA = new MengineRemoteMessageParam("REMOTEMESSAGE_DATA");

    private final String m_name;

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