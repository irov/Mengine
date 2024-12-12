package org.Mengine.Base;

public interface MengineListenerRemoteMessage extends MengineServiceInterface {
    default boolean onMengineRemoteMessageReceived(MengineApplication application, MengineRemoteMessageParam message) {
        //Empty

        return false;
    }

    default void onMengineRemoteMessageDeleted(MengineApplication application) {
        //Empty
    }

    default void onMengineRemoteMessageSent(MengineApplication application, String messageId) {
        //Empty
    }

    default void onMengineRemoteMessageSentError(MengineApplication application, String messageId, Exception exception) {
        //Empty
    }

    default void onMengineRemoteMessageNewToken(MengineApplication application, String token) {
        //Empty
    }
}
