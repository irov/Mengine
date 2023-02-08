package org.Mengine.Plugin.Helpshift;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import org.Mengine.Plugin.FirebaseMessaging.MengineFirebaseMessagingPlugin;
import org.Mengine.Plugin.FirebaseMessaging.MengineFirebaseMessagingListener;

import com.google.firebase.messaging.RemoteMessage;
import com.helpshift.Helpshift;

import androidx.annotation.NonNull;

import java.util.Map;

public class MengineHelpshiftFirebaseMessaging implements MengineHelpshiftMessagingInterface, MengineFirebaseMessagingListener {
    MengineFirebaseMessagingPlugin m_firebaseMessagingPlugin;

    @Override
    public boolean initializeMessaging(MenginePlugin plugin, MengineActivity activity) {
        MengineFirebaseMessagingPlugin firebaseMessagingPlugin = activity.getPlugin(MengineFirebaseMessagingPlugin.class);

        if (firebaseMessagingPlugin == null) {
            return false;
        }

        m_firebaseMessagingPlugin = firebaseMessagingPlugin;

        m_firebaseMessagingPlugin.addMessaging(this);

        return  true;
    }

    @Override
    public void finalizeMessaging(MenginePlugin plugin) {
        m_firebaseMessagingPlugin.removeMessaging(this);
        m_firebaseMessagingPlugin = null;
    }

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
