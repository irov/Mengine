package org.Mengine.Plugin.FirebaseMessaging;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.messaging.FirebaseMessaging;
import com.google.firebase.messaging.RemoteMessage;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;

import android.content.Context;
import android.os.Bundle;

import androidx.annotation.NonNull;

import java.util.ArrayList;

public class MengineFirebaseMessagingPlugin extends MenginePlugin implements MengineFirebaseMessagingListener {
    public static String PLUGIN_NAME = "FirebaseMessaging";
    public static boolean PLUGIN_EMBEDDING = true;

    public ArrayList<MengineFirebaseMessagingListener> m_messagings;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();

        Context context = this.getApplicationContext();
        int resultCode = apiAvailability.isGooglePlayServicesAvailable(context);

        if (resultCode == ConnectionResult.SUCCESS) {
            FirebaseMessaging.getInstance().getToken()
                    .addOnCompleteListener(new OnCompleteListener<String>() {
                        @Override
                        public void onComplete(@NonNull Task<String> task) {
                            if (task.isSuccessful() == false) {
                                MengineFirebaseMessagingPlugin.this.logInfo("Fetching FCM registration token failed: %s"
                                    , task.getException()
                                );

                                return;
                            }

                            // Get new FCM registration token
                            String token = task.getResult();

                            activity.sendEvent("PushToken", token);
                        }
                    });
        }
    }

    public void addMessaging(@NonNull MengineFirebaseMessagingListener listener) {
        m_messagings.add(listener);
    }

    public void removeMessaging(@NonNull MengineFirebaseMessagingListener listener) {
        m_messagings.remove(listener);
    }

    @Override
    public void onMessageReceived(final RemoteMessage remoteMessage) {
        for (MengineFirebaseMessagingListener listener : m_messagings) {
            listener.onMessageReceived(remoteMessage);
        }
    }

    @Override
    public void onDeletedMessages() {
        for (MengineFirebaseMessagingListener listener : m_messagings) {
            listener.onDeletedMessages();
        }
    }

    @Override
    public void onMessageSent(@NonNull String msgId) {
        for (MengineFirebaseMessagingListener listener : m_messagings) {
            listener.onMessageSent(msgId);
        }
    }

    @Override
    public void onSendError(@NonNull String msgId, @NonNull Exception exception) {
        for (MengineFirebaseMessagingListener listener : m_messagings) {
            listener.onSendError(msgId, exception);
        }
    }

    @Override
    public void onNewToken(@NonNull String token) {
        for (MengineFirebaseMessagingListener listener : m_messagings) {
            listener.onNewToken(token);
        }
    }
}