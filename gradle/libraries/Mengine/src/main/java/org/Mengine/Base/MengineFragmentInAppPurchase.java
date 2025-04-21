package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.List;

public class MengineFragmentInAppPurchase extends MengineFragment<MengineListenerInAppPurchase> {
    public static MengineFragmentInAppPurchase INSTANCE = null;

    MengineFragmentInAppPurchase() {
        super(MengineListenerInAppPurchase.class);

        INSTANCE = this;
    }

    public void setInAppProducts(@NonNull List<MengineInAppProductParam> products) {
        this.propagate(MengineListenerInAppPurchase::onMengineSetInAppProducts, products);
    }

    public void purchaseInAppProduct(@NonNull MengineInAppPurchaseParam purchase) {
        long purchase_timestamp = MengineUtils.getTimestamp();

        MengineStatistic.setInteger("purchase.timestamp", purchase_timestamp);

        this.propagate(MengineListenerInAppPurchase::onMenginePurchaseInAppProduct, purchase);
    }
}
