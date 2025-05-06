package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;

public interface MengineServiceInterface {
    default boolean onAvailable(@NonNull MengineApplication application) {
        //Override

        return true;
    }

    boolean onInitialize(@NonNull MengineApplication application, String serviceName, boolean embedding);
    void onFinalize(@NonNull MengineApplication application);

    boolean isEmbedding();

    default void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
        //Override
    }

    default Bundle onSave(@NonNull MengineApplication application) {
        return null;
    }

    String getServiceName();
}
