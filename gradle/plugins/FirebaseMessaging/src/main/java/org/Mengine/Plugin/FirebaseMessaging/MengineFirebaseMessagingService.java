package org.Mengine.Plugin.FirebaseMessaging;

import androidx.annotation.NonNull;

import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineRemoteMessageParam;

import java.util.HashMap;
import java.util.Map;

public class MengineFirebaseMessagingService extends FirebaseMessagingService {
    @Override
    public void onMessageReceived(@NonNull RemoteMessage remoteMessage) {
        super.onMessageReceived(remoteMessage);

        String messageId = remoteMessage.getMessageId();
        String from = remoteMessage.getFrom();
        String to = remoteMessage.getTo();
        String collapseKey = remoteMessage.getCollapseKey();
        Map<String, String> data = remoteMessage.getData();

        MengineRemoteMessageParam message = new MengineRemoteMessageParam();
        message.REMOTEMESSAGE_ID = messageId;
        message.REMOTEMESSAGE_FROM = from;
        message.REMOTEMESSAGE_TO = to;
        message.REMOTEMESSAGE_COLLAPSE_KEY = collapseKey;
        message.REMOTEMESSAGE_DATA = data;

        MengineApplication application = (MengineApplication)this.getApplication();
        application.onMengineRemoteMessageReceived(message);
    }

    @Override
    public void onDeletedMessages() {
        super.onDeletedMessages();

        MengineApplication application = (MengineApplication)this.getApplication();
        application.onMengineRemoteMessageDeleted();
    }

    @Override
    public void onNewToken(@NonNull String token) {
        super.onNewToken(token);

        MengineApplication application = (MengineApplication)this.getApplication();
        application.onMengineRemoteMessageNewToken(token);
    }
}
