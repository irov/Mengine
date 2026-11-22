package org.Mengine.Plugin.GooglePlayBilling;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.android.billingclient.api.BillingClient;

import org.Mengine.Base.MengineOperation;

import java.util.Objects;

final class MengineGooglePlayBillingRestorePurchases extends MengineOperation<Runnable> {
    private final BillingClient m_client;
    private final String m_accountToken;

    MengineGooglePlayBillingRestorePurchases(@NonNull BillingClient client, @Nullable String accountToken) {
        super("restorePurchases");

        m_client = client;
        m_accountToken = accountToken;
    }

    @NonNull
    BillingClient getClient() {
        return m_client;
    }

    boolean isRequest(@NonNull BillingClient client, @Nullable String accountToken) {
        if (m_client != client) {
            return false;
        }

        if (Objects.equals(m_accountToken, accountToken) == false) {
            return false;
        }

        return true;
    }

    @Override
    protected void onComplete(@NonNull Runnable completion) {
        completion.run();
    }
}
