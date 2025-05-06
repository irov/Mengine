package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.List;

public interface MengineListenerInAppPurchase extends MengineServiceInterface {
    void onMengineSetInAppProducts(@NonNull MengineApplication application, @NonNull List<MengineParamInAppProduct> products);
    void onMenginePurchaseInAppProduct(@NonNull MengineApplication application, @NonNull MengineParamInAppPurchase purchase);
}