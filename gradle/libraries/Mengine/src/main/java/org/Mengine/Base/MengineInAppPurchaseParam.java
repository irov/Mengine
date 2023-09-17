package org.Mengine.Base;

public class MengineInAppPurchaseParam {
    public final static MengineInAppPurchaseParam INAPPPURCHASE_TRANSACTION = new MengineInAppPurchaseParam("TRANSACTION");
    public final static MengineInAppPurchaseParam INAPPPURCHASE_PRODUCTS = new MengineInAppPurchaseParam("PRODUCTS");
    public final static MengineInAppPurchaseParam INAPPPURCHASE_ACKNOWLEDGED = new MengineInAppPurchaseParam("ACKNOWLEDGED");
    public final static MengineInAppPurchaseParam INAPPPURCHASE_TOKEN = new MengineInAppPurchaseParam("TOKEN");

    private String m_name;

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