package org.Mengine.Base;

import java.util.Map;

public interface MenginePluginInAppPurchaseListener extends MenginePluginInterface {
    MengineInAppPurchaseParam INAPPPURCHASE_TRANSACTION = MengineInAppPurchaseParam.INAPPPURCHASE_TRANSACTION;
    MengineInAppPurchaseParam INAPPPURCHASE_PRODUCTS = MengineInAppPurchaseParam.INAPPPURCHASE_PRODUCTS;
    MengineInAppPurchaseParam INAPPPURCHASE_ACKNOWLEDGED = MengineInAppPurchaseParam.INAPPPURCHASE_ACKNOWLEDGED;
    MengineInAppPurchaseParam INAPPPURCHASE_TOKEN = MengineInAppPurchaseParam.INAPPPURCHASE_TOKEN;

    void onMengineInAppProduct(MengineApplication application, Map<MengineInAppProductParam, Object> product);
    void onMengineInAppPurchase(MengineApplication application, Map<MengineInAppPurchaseParam, Object> purchase);
}
