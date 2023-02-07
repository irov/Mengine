package org.Mengine.Plugin.FirebaseMessaging;

import androidx.annotation.NonNull;

import com.google.firebase.messaging.RemoteMessage;

public interface MengineFirebaseMessagingListener {
    public boolean onMessageReceived(final RemoteMessage remoteMessage);
    public void onDeletedMessages();
    public void onMessageSent(@NonNull String msgId);
    public void onSendError(@NonNull String msgId, @NonNull Exception exception);
    public void onNewToken(@NonNull String token);
}
