package org.Mengine.Base;

import java.util.Map;

public interface MenginePluginAdRevenueListener extends MenginePluginInterface {
    MengineAdRevenueParam ADREVENUE_MEDIATION = MengineAdRevenueParam.ADREVENUE_MEDIATION;
    MengineAdRevenueParam ADREVENUE_FORMAT = MengineAdRevenueParam.ADREVENUE_FORMAT;
    MengineAdRevenueParam ADREVENUE_ADUNITID = MengineAdRevenueParam.ADREVENUE_ADUNITID;
    MengineAdRevenueParam ADREVENUE_PLACEMENT = MengineAdRevenueParam.ADREVENUE_PLACEMENT;
    MengineAdRevenueParam ADREVENUE_NETWORK = MengineAdRevenueParam.ADREVENUE_NETWORK;
    MengineAdRevenueParam ADREVENUE_REVENUE_PRECISION = MengineAdRevenueParam.ADREVENUE_REVENUE_PRECISION;
    MengineAdRevenueParam ADREVENUE_REVENUE_VALUE = MengineAdRevenueParam.ADREVENUE_REVENUE_VALUE;
    MengineAdRevenueParam ADREVENUE_REVENUE_CURRENCY = MengineAdRevenueParam.ADREVENUE_REVENUE_CURRENCY;

    void onMengineAdRevenue(MengineApplication application, Map<MengineAdRevenueParam, Object> paid);
}
