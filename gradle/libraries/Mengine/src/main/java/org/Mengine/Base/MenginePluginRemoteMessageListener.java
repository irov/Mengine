package org.Mengine.Base;

import java.util.Map;

public interface MenginePluginRemoteMessageListener extends MenginePluginInterface {
    MengineRemoteMessageParam REMOTEMESSAGE_ID = MengineRemoteMessageParam.REMOTEMESSAGE_ID;
    MengineRemoteMessageParam REMOTEMESSAGE_FROM = MengineRemoteMessageParam.REMOTEMESSAGE_FROM;
    MengineRemoteMessageParam REMOTEMESSAGE_TO = MengineRemoteMessageParam.REMOTEMESSAGE_TO;
    MengineRemoteMessageParam REMOTEMESSAGE_COLLAPSE_KEY = MengineRemoteMessageParam.REMOTEMESSAGE_COLLAPSE_KEY;
    MengineRemoteMessageParam REMOTEMESSAGE_DATA = MengineRemoteMessageParam.REMOTEMESSAGE_DATA;

    default boolean onMengineRemoteMessageReceived(MengineApplication application, Map<MengineRemoteMessageParam, Object> message) {
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
