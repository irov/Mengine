package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerConnectivity extends MengineServiceInterface {
    void onMengineConnectivityChanged(@NonNull MengineApplication application, boolean available);
}
