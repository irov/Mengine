package org.Mengine.Plugin.GooglePlayBilling;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineOperation;

final class MengineGooglePlayBillingBuyInApp extends MengineOperation<Runnable> {
    private final String m_productId;

    MengineGooglePlayBillingBuyInApp(@NonNull String productId) {
        super("buyInApp %s", productId);

        m_productId = productId;
    }

    @NonNull
    String getProductId() {
        return m_productId;
    }

    @Override
    protected void onComplete(@NonNull Runnable completion) {
        completion.run();
    }
}
