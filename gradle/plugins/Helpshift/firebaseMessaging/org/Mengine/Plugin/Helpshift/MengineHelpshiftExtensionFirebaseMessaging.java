package org.Mengine.Plugin.Helpshift;

import org.Mengine.Base.MenginePluginExtension;

import org.Mengine.Plugin.FirebaseMessaging.MengineFirebaseMessagingListener;

import com.google.firebase.messaging.RemoteMessage;
import com.helpshift.Helpshift;

import androidx.annotation.NonNull;

import java.util.Map;

public class MengineHelpshiftExtensionFirebaseMessaging extends MenginePluginExtension implements MengineFirebaseMessagingListener {
    @Override
    public boolean onMessageReceived(final RemoteMessage remoteMessage) {
        Map<String, String> data = remoteMessage.getData();
        String origin = data.get("origin");
        if (origin != null && origin.equals("helpshift")) {
            Helpshift.handlePush(data);

            return true;
        }

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
        Helpshift.registerPushToken(token);
    }
}
