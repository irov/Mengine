package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineServiceInterface {
    default boolean onAvailable(@NonNull MengineApplication application) {
        //Override

        return true;
    }

    String getServiceName();
}
