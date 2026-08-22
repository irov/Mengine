package org.Mengine.Base;

public class MengineFragmentConnectivity extends MengineFragment<MengineListenerConnectivity> {
    public static MengineFragmentConnectivity INSTANCE = null;

    MengineFragmentConnectivity() {
        super(MengineListenerConnectivity.class);

        INSTANCE = this;
    }

    public void connectivityChanged(boolean available) {
        this.propagate(MengineListenerConnectivity::onMengineConnectivityChanged, available);
    }
}
