package org.Mengine.Plugin.Adjust;

import android.content.Context;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePluginExtension;

import org.Mengine.Plugin.FirebaseMessaging.MengineFirebaseMessagingListener;

import com.google.firebase.messaging.RemoteMessage;

import com.adjust.sdk.Adjust;

import androidx.annotation.NonNull;

public class MengineAdjustExtensionFirebaseMessaging extends MenginePluginExtension implements MengineFirebaseMessagingListener {
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
        MengineActivity activity = this.getActivity();

        final Context context = activity.getApplicationContext();

        Adjust.setPushToken(token, context);
    }
}
