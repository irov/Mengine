package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerRemoteMessage extends MengineServiceInterface {
    default boolean onMengineRemoteMessageReceived(@NonNull MengineApplication application, @NonNull MengineParamRemoteMessage message) {
        //Empty

        return false;
    }

    default void onMengineRemoteMessageDeleted(@NonNull MengineApplication application) {
        //Empty
    }

    default void onMengineRemoteMessageNewToken(@NonNull MengineApplication application, String token) {
        //Empty
    }
}
