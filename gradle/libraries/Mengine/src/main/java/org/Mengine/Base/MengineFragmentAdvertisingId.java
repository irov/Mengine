package org.Mengine.Base;

public class MengineFragmentAdvertisingId extends MengineFragment<MengineListenerAdvertisingId> {
    public static MengineFragmentAdvertisingId INSTANCE = null;

    public String m_advertisingId;
    public boolean m_limitAdTracking;

    public String getAdvertisingId() {
        return m_advertisingId;
    }

    public boolean isLimitAdTracking() {
        return m_limitAdTracking;
    }

    MengineFragmentAdvertisingId() {
        super(MengineListenerAdvertisingId.class);

        INSTANCE = this;
    }

    public void setAdvertisingId(String advertisingId, boolean limitAdTracking) {
        m_advertisingId = advertisingId;
        m_limitAdTracking = limitAdTracking;

        MengineApplication.INSTANCE.setState("user.limit_ad_tracking", limitAdTracking);

        this.propagate(MengineListenerAdvertisingId::onMengineAdvertisingId, advertisingId, limitAdTracking);
    }
}