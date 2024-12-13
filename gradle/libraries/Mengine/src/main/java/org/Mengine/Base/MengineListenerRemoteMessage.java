package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerRemoteMessage extends MengineServiceInterface {
    default boolean onMengineRemoteMessageReceived(@NonNull MengineApplication application, MengineRemoteMessageParam message) {
        //Empty

        return false;
    }

    default void onMengineRemoteMessageDeleted(@NonNull MengineApplication application) {
        //Empty
    }

    default void onMengineRemoteMessageSent(@NonNull MengineApplication application, String messageId) {
        //Empty
    }

    default void onMengineRemoteMessageSentError(@NonNull MengineApplication application, String messageId, Exception exception) {
        //Empty
    }

    default void onMengineRemoteMessageNewToken(@NonNull MengineApplication application, String token) {
        //Empty
    }
}
