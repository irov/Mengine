package org.Mengine.Base;

public interface MengineListenerAdvertisingId extends MengineServiceInterface {
    void onMengineAdvertisingId(MengineApplication application, String advertisingId, boolean limitAdTracking);
}