package org.Mengine.Base;

public class MengineFragmentAdvertisingId extends MengineFragment<MengineListenerAdvertisingId> {
    public static MengineFragmentAdvertisingId INSTANCE = null;

    MengineFragmentAdvertisingId() {
        super(MengineListenerAdvertisingId.class);

        INSTANCE = this;
    }

    public void setAdvertisingId(String advertisingId, boolean limitAdTracking) {
        MengineApplication.INSTANCE.setState("user.limit_ad_tracking", limitAdTracking);

        this.propagate(MengineListenerAdvertisingId::onMengineAdvertisingId, advertisingId, limitAdTracking);
    }
}