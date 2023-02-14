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
import org.Mengine.Base.MenginePluginExtension;
import org.Mengine.Base.MenginePluginExtensionListener;

import android.content.Context;
import android.os.Bundle;

import androidx.annotation.NonNull;

import java.util.ArrayList;

public class MengineFirebaseMessagingPlugin extends MenginePlugin implements MenginePluginExtensionListener {
    public static String PLUGIN_NAME = "FirebaseMessaging";
    public static boolean PLUGIN_EMBEDDING = true;

    public ArrayList<MengineFirebaseMessagingListener> m_messagings;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();

        Context context = activity.getApplicationContext();
        int resultCode = apiAvailability.isGooglePlayServicesAvailable(context);

        if (resultCode == ConnectionResult.SUCCESS) {
            FirebaseMessaging.getInstance().getToken()
                .addOnCompleteListener(new OnCompleteListener<String>() {
                    @Override
                    public void onComplete(@NonNull Task<String> task) {
                        if (task.isSuccessful() == false) {
                            MengineFirebaseMessagingPlugin.this.logError("fetching FCM registration token failed: %s"
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
        for (MengineFirebaseMessagingListener listener : m_messagings) {
            listener.onNewToken(token);
        }
    }
}