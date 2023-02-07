package org.Mengine.Plugin.FirebaseMessaging;

import androidx.annotation.NonNull;

import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

import org.Mengine.Base.MengineApplication;

public class MengineFirebaseMessagingService extends FirebaseMessagingService {
    @Override
    public void onMessageReceived(@NonNull RemoteMessage remoteMessage) {
        super.onMessageReceived(remoteMessage);

        MengineApplication application = (MengineApplication)this.getApplication();

        MengineFirebaseMessagingPlugin plugin = application.getPlugin(MengineFirebaseMessagingPlugin.class);

        plugin.onMessageReceived(remoteMessage);
    }

    @Override
    public void onDeletedMessages() {
        super.onDeletedMessages();

        MengineApplication application = (MengineApplication)this.getApplication();

        MengineFirebaseMessagingPlugin plugin = application.getPlugin(MengineFirebaseMessagingPlugin.class);

        plugin.onDeletedMessages();
    }

    @Override
    public void onMessageSent(@NonNull String msgId) {
        super.onMessageSent(msgId);

        MengineApplication application = (MengineApplication)this.getApplication();

        MengineFirebaseMessagingPlugin plugin = application.getPlugin(MengineFirebaseMessagingPlugin.class);

        plugin.onMessageSent(msgId);
    }

    @Override
    public void onSendError(@NonNull String msgId, @NonNull Exception exception) {
        super.onSendError(msgId, exception);

        MengineApplication application = (MengineApplication)this.getApplication();

        MengineFirebaseMessagingPlugin plugin = application.getPlugin(MengineFirebaseMessagingPlugin.class);

        plugin.onSendError(msgId, exception);
    }

    @Override
    public void onNewToken(@NonNull String token) {
        super.onNewToken(token);

        MengineApplication application = (MengineApplication)this.getApplication();

        MengineFirebaseMessagingPlugin plugin = application.getPlugin(MengineFirebaseMessagingPlugin.class);

        plugin.onNewToken(token);
    }
}
