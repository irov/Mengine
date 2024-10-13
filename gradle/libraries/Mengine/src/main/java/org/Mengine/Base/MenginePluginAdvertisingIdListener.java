package org.Mengine.Base;

public interface MenginePluginAdvertisingIdListener extends MenginePluginInterface {
    void onMengineAdvertisingId(MengineApplication application, String advertisingId, boolean limitAdTracking);
}