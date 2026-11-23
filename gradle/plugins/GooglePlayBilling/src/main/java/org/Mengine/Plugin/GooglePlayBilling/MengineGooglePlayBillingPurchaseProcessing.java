package org.Mengine.Plugin.GooglePlayBilling;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineOperation;

import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;

final class MengineGooglePlayBillingPurchaseProcessing extends MengineOperation<Boolean> {
    private final Runnable m_release;
    private final List<Consumer<Boolean>> m_completions = new ArrayList<>();

    MengineGooglePlayBillingPurchaseProcessing(@NonNull String token, @NonNull Runnable release) {
        super("purchase %s", token);

        m_release = release;
    }

    void addCompletion(@NonNull Consumer<Boolean> completion) {
        m_completions.add(completion);
    }

    @Override
    protected void onComplete(Boolean successful) {
        m_release.run();

        for (Consumer<Boolean> completion : m_completions) {
            completion.accept(successful);
        }
    }
}
