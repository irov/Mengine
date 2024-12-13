package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerAdvertisingId extends MengineServiceInterface {
    void onMengineAdvertisingId(@NonNull MengineApplication application, String advertisingId, boolean limitAdTracking);
}