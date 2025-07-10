package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.List;
import java.util.Set;

public interface MengineListenerInAppPurchase extends MengineServiceInterface {
    default void onMengineSetInAppProducts(@NonNull MengineApplication application, @NonNull List<MengineParamInAppProduct> products) {
        //Empty
    }

    default void onMenginePurchaseInAppProduct(@NonNull MengineApplication application, @NonNull MengineParamInAppPurchase purchase) {
        //Empty
    }

    default void onMengineOwnedInAppProducts(@NonNull MengineApplication application, @NonNull Set<String> products) {
        //Empty
    }
}