package org.Mengine.Plugin.FirebaseMessaging;

import com.google.firebase.messaging.RemoteMessage;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginExtension;
import org.Mengine.Base.MenginePluginExtensionListener;

import android.os.Bundle;

import androidx.annotation.NonNull;

import java.util.ArrayList;

public class MengineFirebaseMessagingPlugin extends MenginePlugin implements MenginePluginExtensionListener {
    public static final String PLUGIN_NAME = "FirebaseMessaging";
    public static final boolean PLUGIN_EMBEDDING = true;

    private ArrayList<MengineFirebaseMessagingListener> m_messagings = new ArrayList<>();

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        String token = this.getPrivateSettingString("PushToken", null);

        if (token != null) {
            this.sendPushTokenEvent(token);
        }
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        m_messagings = null;
    }

    @Override
    public void onMenginePluginExtension(MengineApplication application, MengineActivity activity, MenginePlugin plugin, MenginePluginExtension extension) {
        if (extension instanceof MengineFirebaseMessagingListener) {
            MengineFirebaseMessagingListener listener = (MengineFirebaseMessagingListener)extension;

            m_messagings.add(listener);
        }
    }

    public void onMessageReceived(final RemoteMessage remoteMessage) {
        for (MengineFirebaseMessagingListener listener : m_messagings) {
            if (listener.onMessageReceived(remoteMessage) == true) {
                break;
            }
        }
    }

    public void onDeletedMessages() {
        for (MengineFirebaseMessagingListener listener : m_messagings) {
            listener.onDeletedMessages();
        }
    }

    public void onMessageSent(@NonNull String msgId) {
        for (MengineFirebaseMessagingListener listener : m_messagings) {
            listener.onMessageSent(msgId);
        }
    }

    public void onSendError(@NonNull String msgId, @NonNull Exception exception) {
        for (MengineFirebaseMessagingListener listener : m_messagings) {
            listener.onSendError(msgId, exception);
        }
    }

    public void onNewToken(@NonNull String token) {
        this.setPrivateSettingString("PushToken", token);

        this.sendPushTokenEvent(token);

        for (MengineFirebaseMessagingListener listener : m_messagings) {
            listener.onNewToken(token);
        }
    }

    private void sendPushTokenEvent(String token) {
        MengineActivity activity = this.getActivity();

        activity.sendEvent("PushToken", token);
    }
}