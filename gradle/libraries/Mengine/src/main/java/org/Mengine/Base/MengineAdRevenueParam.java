package org.Mengine.Base;

public class MengineAdRevenueParam {
    public final static MengineAdRevenueParam ADREVENUE_MEDIATION = new MengineAdRevenueParam("MEDIATION");
    public final static MengineAdRevenueParam ADREVENUE_FORMAT = new MengineAdRevenueParam("FORMAT");
    public final static MengineAdRevenueParam ADREVENUE_ADUNITID = new MengineAdRevenueParam("ADUNITID");
    public final static MengineAdRevenueParam ADREVENUE_PLACEMENT = new MengineAdRevenueParam("PLACEMENT");
    public final static MengineAdRevenueParam ADREVENUE_NETWORK = new MengineAdRevenueParam("NETWORK");
    public final static MengineAdRevenueParam ADREVENUE_REVENUE_PRECISION = new MengineAdRevenueParam("REVENUE_PRECISION");
    public final static MengineAdRevenueParam ADREVENUE_REVENUE_VALUE = new MengineAdRevenueParam("REVENUE_VALUE");
    public final static MengineAdRevenueParam ADREVENUE_REVENUE_CURRENCY = new MengineAdRevenueParam("REVENUE_CURRENCY");

    private String m_name;

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