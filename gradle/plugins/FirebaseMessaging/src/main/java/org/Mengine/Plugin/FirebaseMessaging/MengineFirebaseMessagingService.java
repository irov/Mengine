package org.Mengine.Plugin.FirebaseMessaging;

import androidx.annotation.NonNull;

import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

import org.Mengine.Base.MengineFragmentRemoteMessage;
import org.Mengine.Base.MengineParamRemoteMessage;

import java.util.HashMap;
import java.util.Map;

public class MengineFirebaseMessagingService extends FirebaseMessagingService {
    @Override
    public void onMessageReceived(@NonNull RemoteMessage remoteMessage) {
        super.onMessageReceived(remoteMessage);

        String messageId = remoteMessage.getMessageId();
        String from = remoteMessage.getFrom();
        String collapseKey = remoteMessage.getCollapseKey();
        Map<String, String> data = remoteMessage.getData();

        MengineParamRemoteMessage message = new MengineParamRemoteMessage();
        message.REMOTEMESSAGE_ID = messageId;
        message.REMOTEMESSAGE_FROM = from;
        message.REMOTEMESSAGE_COLLAPSE_KEY = collapseKey;

        message.REMOTEMESSAGE_DATA = new HashMap<>();
        message.REMOTEMESSAGE_DATA.putAll(data);

        MengineFragmentRemoteMessage.INSTANCE.remoteMessageReceived(message);
    }

    @Override
    public void onDeletedMessages() {
        super.onDeletedMessages();

        MengineFragmentRemoteMessage.INSTANCE.remoteMessageDeleted();
    }

    @Override
    public void onNewToken(@NonNull String token) {
        super.onNewToken(token);

        MengineFragmentRemoteMessage.INSTANCE.remoteMessageNewToken(token);
    }
}
