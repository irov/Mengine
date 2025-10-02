package org.Mengine.Base;

public class MengineFragmentAdvertisingId extends MengineFragment<MengineListenerAdvertisingId> {
    public static MengineFragmentAdvertisingId INSTANCE = null;

    public static final String LIMIT_ADVERTISING_ID = "00000000-0000-0000-0000-000000000000";

    public String m_advertisingId = MengineFragmentAdvertisingId.LIMIT_ADVERTISING_ID;
    public boolean m_limitAdTracking = true;

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