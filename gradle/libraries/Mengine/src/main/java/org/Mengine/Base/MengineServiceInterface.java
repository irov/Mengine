package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;

public interface MengineServiceInterface {
    default boolean onAvailable(@NonNull MengineApplication application) {
        //Override

        return true;
    }

    void onAppInitialize(@NonNull MengineApplication application, String serviceName, boolean embedding);
    void onAppFinalize(@NonNull MengineApplication application);

    void onActivityInitialize(@NonNull MengineActivity activity);
    void onActivityFinalize(@NonNull MengineActivity activity);

    boolean isEmbedding();

    default void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
        //Override
    }

    default Bundle onSave(@NonNull MengineApplication application) {
        return null;
    }

    String getServiceName();
    String getServiceTag();
}
