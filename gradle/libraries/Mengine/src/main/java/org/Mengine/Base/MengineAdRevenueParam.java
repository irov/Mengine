package org.Mengine.Base;

public class MengineAdRevenueParam {
    public final static MengineAdRevenueParam ADREVENUE_MEDIATION = new MengineAdRevenueParam("ADREVENUE_MEDIATION");
    public final static MengineAdRevenueParam ADREVENUE_FORMAT = new MengineAdRevenueParam("ADREVENUE_FORMAT");
    public final static MengineAdRevenueParam ADREVENUE_ADUNITID = new MengineAdRevenueParam("ADREVENUE_ADUNITID");
    public final static MengineAdRevenueParam ADREVENUE_PLACEMENT = new MengineAdRevenueParam("ADREVENUE_PLACEMENT");
    public final static MengineAdRevenueParam ADREVENUE_NETWORK = new MengineAdRevenueParam("ADREVENUE_NETWORK");
    public final static MengineAdRevenueParam ADREVENUE_REVENUE_PRECISION = new MengineAdRevenueParam("ADREVENUE_REVENUE_PRECISION");
    public final static MengineAdRevenueParam ADREVENUE_REVENUE_VALUE = new MengineAdRevenueParam("ADREVENUE_REVENUE_VALUE");
    public final static MengineAdRevenueParam ADREVENUE_REVENUE_CURRENCY = new MengineAdRevenueParam("ADREVENUE_REVENUE_CURRENCY");

    private final String m_name;

    MengineAdRevenueParam(String name) {
        m_name = name;
    }

    public String getName() {
        return m_name;
    }
    public String toString() {
        return m_name;
    }
}