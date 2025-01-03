package org.Mengine.Base;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.os.Build;

import androidx.annotation.NonNull;

public class MengineMonitorConnectivityStatus extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "MConnStatus";

    private ConnectivityManager.NetworkCallback m_networkCallback;

    @Override
    public void onAppCreate(@NonNull MengineApplication application) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
            return;
        }

        ConnectivityManager.NetworkCallback networkCallback = new ConnectivityManager.NetworkCallback() {
            @Override
            public void onAvailable(@NonNull Network network) {
                super.onAvailable(network);

                MengineMonitorConnectivityStatus.this.logMessage("NetworkCallback onAvailable");

                MengineNetwork.setNetworkAvailable(true);
            }

            @Override
            public void onLost(@NonNull Network network) {
                super.onLost(network);

                MengineMonitorConnectivityStatus.this.logMessage("NetworkCallback onLost");

                MengineNetwork.setNetworkAvailable(false);
            }

            private MengineNetworkTransport getNetworkTransport(@NonNull NetworkCapabilities networkCapabilities) {
                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_CELLULAR;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_WIFI;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_BLUETOOTH) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_BLUETOOTH;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_ETHERNET) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_ETHERNET;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_VPN) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_VPN;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI_AWARE) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_WIFI_AWARE;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_LOWPAN) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_LOWPAN;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_USB) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_USB;
                }

                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_THREAD) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_THREAD;
                }

                return MengineNetworkTransport.NETWORKTRANSPORT_UNKNOWN;
            }

            @Override
            public void onCapabilitiesChanged(@NonNull Network network, @NonNull NetworkCapabilities networkCapabilities) {
                super.onCapabilitiesChanged(network, networkCapabilities);
                final boolean unmetered = networkCapabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_NOT_METERED);

                MengineNetworkTransport transport = this.getNetworkTransport(networkCapabilities);

                boolean last_unmetered = MengineNetwork.isNetworkUnmetered();
                MengineNetworkTransport last_transport = MengineNetwork.getNetworkTransport();

                if (last_unmetered == unmetered && last_transport == transport) {
                    return;
                }

                MengineMonitorConnectivityStatus.this.logMessage("NetworkCallback onCapabilitiesChanged unmetered: %b transport: %s"
                    , unmetered
                    , transport
                );

                MengineNetwork.setNetworkUnmetered(unmetered);
                MengineNetwork.setNetworkTransport(transport);
            }
        };

        Context context = application.getApplicationContext();

        ConnectivityManager connectivityManager = context.getSystemService(ConnectivityManager.class);

        if (connectivityManager != null) {
            connectivityManager.registerDefaultNetworkCallback(networkCallback);
        }

        m_networkCallback = networkCallback;
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
            return;
        }

        if (m_networkCallback == null) {
            return;
        }

        Context context = application.getApplicationContext();

        ConnectivityManager connectivityManager = context.getSystemService(ConnectivityManager.class);

        if (connectivityManager != null) {
            connectivityManager.unregisterNetworkCallback(m_networkCallback);
        }

        m_networkCallback = null;
    }
}