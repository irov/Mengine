package org.Mengine.Base;

import java.util.Map;

public interface MenginePluginRemoteMessageListener extends MenginePluginInterface {
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
