package org.Mengine.Base;

public class MengineInAppPurchaseParam {
    public final static MengineInAppPurchaseParam INAPPPURCHASE_TRANSACTION = new MengineInAppPurchaseParam("INAPPPURCHASE_TRANSACTION");
    public final static MengineInAppPurchaseParam INAPPPURCHASE_PRODUCTS = new MengineInAppPurchaseParam("INAPPPURCHASE_PRODUCTS");
    public final static MengineInAppPurchaseParam INAPPPURCHASE_QUANTITY = new MengineInAppPurchaseParam("INAPPPURCHASE_QUANTITY");
    public final static MengineInAppPurchaseParam INAPPPURCHASE_ACKNOWLEDGED = new MengineInAppPurchaseParam("INAPPPURCHASE_ACKNOWLEDGED");
    public final static MengineInAppPurchaseParam INAPPPURCHASE_TOKEN = new MengineInAppPurchaseParam("INAPPPURCHASE_TOKEN");
    public final static MengineInAppPurchaseParam INAPPPURCHASE_DATA = new MengineInAppPurchaseParam("INAPPPURCHASE_DATA");

    private final String m_name;

    MengineInAppPurchaseParam(String name) {
        m_name = name;
    }

    public String getName() {
        return m_name;
    }
    public String toString() {
        return m_name;
    }
}