package org.Mengine.Base;

import java.util.List;

public interface MenginePluginInAppPurchaseListener extends MenginePluginInterface {
    void onMengineInAppProduct(MengineApplication application, List<MengineInAppProductParam> products);
    void onMengineInAppPurchase(MengineApplication application, MengineInAppPurchaseParam purchase);
}
