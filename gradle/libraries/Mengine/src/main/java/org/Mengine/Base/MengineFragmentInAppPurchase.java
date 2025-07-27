package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.List;
import java.util.Set;

public class MengineFragmentInAppPurchase extends MengineFragment<MengineListenerInAppPurchase> {
    public static MengineFragmentInAppPurchase INSTANCE = null;

    MengineFragmentInAppPurchase() {
        super(MengineListenerInAppPurchase.class);

        INSTANCE = this;
    }

    public void setInAppProducts(@NonNull List<MengineParamInAppProduct> products) {
        this.propagate(MengineListenerInAppPurchase::onMengineSetInAppProducts, products);
    }

    public void purchaseInAppProduct(@NonNull MengineParamInAppPurchase purchase) {
        long purchase_timestamp = MengineUtils.getTimestamp();

        MengineStatistic.increaseInteger("purchase.count", 1);
        MengineStatistic.setInteger("purchase.timestamp", purchase_timestamp);

        this.propagate(MengineListenerInAppPurchase::onMenginePurchaseInAppProduct, purchase);
    }

    public boolean isOwnedInAppProduct(@NonNull String product) {
        Set<String> ownedProducts = MenginePreferences.getPreferenceStrings("mengine.billing.purchase.owned.products", null);

        if (ownedProducts == null) {
            return false;
        }

        if (ownedProducts.isEmpty() == true) {
            return false;
        }

        return ownedProducts.contains(product);
    }

    public void ownedInAppProducts(@NonNull Set<String> products) {
        MenginePreferences.setPreferenceStrings("mengine.billing.purchase.owned.products", products);

        this.propagate(MengineListenerInAppPurchase::onMengineOwnedInAppProducts, products);
    }
}
