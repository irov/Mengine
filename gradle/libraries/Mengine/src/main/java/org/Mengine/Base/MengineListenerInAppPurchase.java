package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.List;

public interface MengineListenerInAppPurchase extends MengineServiceInterface {
    void onMengineInAppProduct(@NonNull MengineApplication application, List<MengineInAppProductParam> products);
    void onMengineInAppPurchase(@NonNull MengineApplication application, MengineInAppPurchaseParam purchase);
}
