package org.Mengine.Base;

public class MengineAdFormat {
    public final static MengineAdFormat ADFORMAT_BANNER = new MengineAdFormat("ADFORMAT_BANNER");
    public final static MengineAdFormat ADFORMAT_MREC = new MengineAdFormat("ADFORMAT_MREC");
    public final static MengineAdFormat ADFORMAT_LEADER = new MengineAdFormat("ADFORMAT_LEADER");
    public final static MengineAdFormat ADFORMAT_INTERSTITIAL = new MengineAdFormat("ADFORMAT_INTERSTITIAL");
    public final static MengineAdFormat ADFORMAT_APP_OPEN = new MengineAdFormat("ADFORMAT_APP_OPEN");
    public final static MengineAdFormat ADFORMAT_REWARDED = new MengineAdFormat("ADFORMAT_REWARDED");
    public final static MengineAdFormat ADFORMAT_REWARDED_INTERSTITIAL = new MengineAdFormat("ADFORMAT_REWARDED_INTERSTITIAL");
    public final static MengineAdFormat ADFORMAT_NATIVE = new MengineAdFormat("ADFORMAT_NATIVE");
    public final static MengineAdFormat ADFORMAT_CROSS_PROMO = new MengineAdFormat("ADFORMAT_CROSS_PROMO");
    public final static MengineAdFormat ADFORMAT_UNKNOWN = new MengineAdFormat("ADFORMAT_UNKNOWN");

    private final String m_name;

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