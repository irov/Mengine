package org.Mengine.Base;

public class MengineAdFormat {
    public final static MengineAdFormat ADFORMAT_BANNER = new MengineAdFormat("BANNER");
    public final static MengineAdFormat ADFORMAT_MREC = new MengineAdFormat("MREC");
    public final static MengineAdFormat ADFORMAT_LEADER = new MengineAdFormat("LEADER");
    public final static MengineAdFormat ADFORMAT_INTERSTITIAL = new MengineAdFormat("INTERSTITIAL");
    public final static MengineAdFormat ADFORMAT_APP_OPEN = new MengineAdFormat("APP_OPEN");
    public final static MengineAdFormat ADFORMAT_REWARDED = new MengineAdFormat("REWARDED");
    public final static MengineAdFormat ADFORMAT_REWARDED_INTERSTITIAL = new MengineAdFormat("REWARDED_INTERSTITIAL");
    public final static MengineAdFormat ADFORMAT_NATIVE = new MengineAdFormat("NATIVE");
    public final static MengineAdFormat ADFORMAT_CROSS_PROMO = new MengineAdFormat("CROSS_PROMO");
    public final static MengineAdFormat ADFORMAT_UNKNOWN = new MengineAdFormat("UNKNOWN");

    private String m_name;

    MengineAdFormat(String name) {
        m_name = name;
    }

    public String getName() {
        return m_name;
    }
    public String toString() {
        return m_name;
    }
}