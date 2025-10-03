package org.Mengine.Base;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.Build;
import android.os.ext.SdkExtensions;

import androidx.annotation.NonNull;

public class MengineMonitorConnectivityStatusService extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "MonitorConnStatus";

    private ConnectivityManager.NetworkCallback m_networkCallback;

    private static MengineNetworkTransport getNetworkTransport(@NonNull NetworkCapabilities networkCapabilities) {
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

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI_AWARE) == true) {
                return MengineNetworkTransport.NETWORKTRANSPORT_WIFI_AWARE;
            }
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O_MR1) {
            if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_LOWPAN) == true) {
                return MengineNetworkTransport.NETWORKTRANSPORT_LOWPAN;
            }
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (SdkExtensions.getExtensionVersion(Build.VERSION_CODES.UPSIDE_DOWN_CAKE) >= 12) {
                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_USB) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_USB;
                }
            }
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (SdkExtensions.getExtensionVersion(Build.VERSION_CODES.UPSIDE_DOWN_CAKE) >= 7) {
                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_THREAD) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_THREAD;
                }
            }
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
            if (SdkExtensions.getExtensionVersion(Build.VERSION_CODES.UPSIDE_DOWN_CAKE) >= 12) {
                if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_SATELLITE) == true) {
                    return MengineNetworkTransport.NETWORKTRANSPORT_SATELLITE;
                }
            }
        }

        return MengineNetworkTransport.NETWORKTRANSPORT_UNKNOWN;
    }

    private void recomputeAndPublish(@NonNull ConnectivityManager cm) {
        boolean available = false;
        boolean unmetered = false;
        MengineNetworkTransport transport = MengineNetworkTransport.NETWORKTRANSPORT_UNKNOWN;

        try {
            Network[] networks = cm.getAllNetworks();

            for (Network network : networks) {
                NetworkCapabilities nc = cm.getNetworkCapabilities(network);

                if (nc != null &&
                    nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET) == true &&
                    nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_VALIDATED) == true ) {

                    available = true;
                    unmetered = nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_NOT_METERED);
                    transport = MengineMonitorConnectivityStatusService.getNetworkTransport(nc);

                    break;
                }
            }
        } catch (SecurityException e) {
            MengineMonitorConnectivityStatusService.this.logError("SecurityException ACCESS_NETWORK_STATE missing [exception: %s]"
                , e.getMessage()
            );
        }

        MengineNetwork.setNetworkAvailable(available);
        MengineNetwork.setNetworkUnmetered(unmetered);
        MengineNetwork.setNetworkTransport(transport);
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) {
        Context context = application.getApplicationContext();
        ConnectivityManager cm = context.getSystemService(ConnectivityManager.class);

        if (cm == null) {
            this.logError("invalid get ConnectivityManager");

            return;
        }

        ConnectivityManager.NetworkCallback networkCallback = new ConnectivityManager.NetworkCallback() {
            @Override
            public void onAvailable(@NonNull Network network) {
                super.onAvailable(network);

                MengineMonitorConnectivityStatusService.this.logInfo("network %s available"
                    , network.toString()
                );

                MengineMonitorConnectivityStatusService.this.recomputeAndPublish(cm);
            }

            @Override
            public void onLost(@NonNull Network network) {
                super.onLost(network);

                MengineMonitorConnectivityStatusService.this.logInfo("network %s lost"
                    , network.toString()
                );

                MengineMonitorConnectivityStatusService.this.recomputeAndPublish(cm);
            }

            @Override
            public void onCapabilitiesChanged(@NonNull Network network, @NonNull NetworkCapabilities networkCapabilities) {
                super.onCapabilitiesChanged(network, networkCapabilities);
                final boolean unmetered = networkCapabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_NOT_METERED);
                MengineNetworkTransport transport = MengineMonitorConnectivityStatusService.getNetworkTransport(networkCapabilities);

                MengineMonitorConnectivityStatusService.this.logInfo("network %s capabilities changed unmetered: %b transport: %s"
                    , network.toString()
                    , unmetered
                    , transport
                );

                MengineMonitorConnectivityStatusService.this.recomputeAndPublish(cm);
            }
        };

        try {
            NetworkRequest request = new NetworkRequest.Builder()
                .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
                .build();

            cm.registerNetworkCallback(request, networkCallback);

            m_networkCallback = networkCallback;
        } catch (Exception e) {
            this.logError("invalid registerNetworkCallback: %s"
                , e.getMessage()
            );
        }

        this.recomputeAndPublish(cm);
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        if (m_networkCallback == null) {
            return;
        }

        Context context = application.getApplicationContext();

        ConnectivityManager cm = context.getSystemService(ConnectivityManager.class);

        if (cm != null) {
            try {
                cm.unregisterNetworkCallback(m_networkCallback);
            } catch (Exception e) {
                this.logError("invalid unregisterNetworkCallback: %s"
                    , e.getMessage()
                );
            }
        }

        m_networkCallback = null;
    }
}