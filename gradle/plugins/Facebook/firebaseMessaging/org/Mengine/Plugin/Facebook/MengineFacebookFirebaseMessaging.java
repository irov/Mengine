package org.Mengine.Plugin.Facebook;

import org.Mengine.Base.MenginePluginExtension;

import org.Mengine.Plugin.FirebaseMessaging.MengineFirebaseMessagingListener;

import com.google.firebase.messaging.RemoteMessage;

import com.facebook.appevents.AppEventsLogger;

import androidx.annotation.NonNull;

public class MengineFacebookFirebaseMessaging extends MenginePluginExtension implements MengineFirebaseMessagingListener {
    @Override
    public boolean onMessageReceived(final RemoteMessage remoteMessage) {

        return false;
    }

    @Override
    public void onDeletedMessages() {
    }

    @Override
    public void onMessageSent(@NonNull String msgId) {
    }

    @Override
    public void onSendError(@NonNull String msgId, @NonNull Exception exception) {
    }

    @Override
    public void onNewToken(@NonNull String token) {
        AppEventsLogger.setPushNotificationsRegistrationId(token);
    }
}
