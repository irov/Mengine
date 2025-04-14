package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.List;

public interface MengineListenerInAppPurchase extends MengineServiceInterface {
    void onMengineSetInAppProducts(@NonNull MengineApplication application, List<MengineInAppProductParam> products);
    void onMenginePurchaseInAppProduct(@NonNull MengineApplication application, MengineInAppPurchaseParam purchase);
}