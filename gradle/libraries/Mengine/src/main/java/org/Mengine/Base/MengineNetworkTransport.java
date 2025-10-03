package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineNetworkTransport {
    public final static MengineNetworkTransport NETWORKTRANSPORT_CELLULAR = new MengineNetworkTransport("NETWORKTRANSPORT_CELLULAR");
    public final static MengineNetworkTransport NETWORKTRANSPORT_WIFI = new MengineNetworkTransport("NETWORKTRANSPORT_WIFI");
    public final static MengineNetworkTransport NETWORKTRANSPORT_BLUETOOTH = new MengineNetworkTransport("NETWORKTRANSPORT_BLUETOOTH");
    public final static MengineNetworkTransport NETWORKTRANSPORT_ETHERNET = new MengineNetworkTransport("NETWORKTRANSPORT_ETHERNET");
    public final static MengineNetworkTransport NETWORKTRANSPORT_VPN = new MengineNetworkTransport("NETWORKTRANSPORT_VPN");
    public final static MengineNetworkTransport NETWORKTRANSPORT_WIFI_AWARE = new MengineNetworkTransport("NETWORKTRANSPORT_WIFI_AWARE");
    public final static MengineNetworkTransport NETWORKTRANSPORT_LOWPAN = new MengineNetworkTransport("NETWORKTRANSPORT_LOWPAN");
    public final static MengineNetworkTransport NETWORKTRANSPORT_USB = new MengineNetworkTransport("NETWORKTRANSPORT_USB");
    public final static MengineNetworkTransport NETWORKTRANSPORT_THREAD = new MengineNetworkTransport("NETWORKTRANSPORT_THREAD");
    public final static MengineNetworkTransport NETWORKTRANSPORT_SATELLITE = new MengineNetworkTransport("NETWORKTRANSPORT_SATELLITE");
    public final static MengineNetworkTransport NETWORKTRANSPORT_UNKNOWN = new MengineNetworkTransport("NETWORKTRANSPORT_UNKNOWN");

    private final String m_name;

    MengineNetworkTransport(@NonNull String name) {
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