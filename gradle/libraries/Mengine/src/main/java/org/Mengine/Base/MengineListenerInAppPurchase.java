package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.List;

public interface MengineListenerInAppPurchase extends MengineServiceInterface {
    void onMengineSetInAppProducts(@NonNull MengineApplication application, @NonNull List<MengineInAppProductParam> products);
    void onMenginePurchaseInAppProduct(@NonNull MengineApplication application, @NonNull MengineInAppPurchaseParam purchase);
}