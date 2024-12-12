package org.Mengine.Base;

import java.util.List;

public interface MengineListenerInAppPurchase extends MengineServiceInterface {
    void onMengineInAppProduct(MengineApplication application, List<MengineInAppProductParam> products);
    void onMengineInAppPurchase(MengineApplication application, MengineInAppPurchaseParam purchase);
}
