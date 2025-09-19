package org.Mengine.Plugin.FirebaseMessaging;

import com.google.firebase.messaging.FirebaseMessaging;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFragmentPushToken;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import android.Manifest;
import android.os.Bundle;

import androidx.annotation.NonNull;

import org.Mengine.Plugin.GoogleService.MengineGoogleServicePlugin;

import java.util.Map;

public class MengineFirebaseMessagingPlugin extends MengineService implements MengineListenerActivity {
    public static final String SERVICE_NAME = "FBMessaging";
    public static final boolean SERVICE_EMBEDDING = true;

    @Override
    public boolean onAvailable(@NonNull MengineApplication application) {
        if (MengineGoogleServicePlugin.isGooglePlayServicesAvailable(application) == false) {
            return false;
        }

        return true;
    }

    public void askNotificationPermission(@NonNull MengineActivity activity) {
        activity.checkPermissionPostNotifications();
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        this.askNotificationPermission(activity);

        FirebaseMessaging firebaseMessaging = FirebaseMessaging.getInstance();

        firebaseMessaging.getToken()
            .addOnCompleteListener(task -> {
                if (task.isSuccessful() == false) {
                    Exception exception = task.getException();

                    if (exception != null) {
                        this.logException(exception, Map.of());
                    } else {
                        this.logError("fetching FCM registration token failed: unknown error");
                    }

                    return;
                }

                // Get new FCM registration token
                String token = task.getResult();

                this.logInfo("fetching FCM registration token success: %s"
                    , token
                );

                MengineFragmentPushToken.INSTANCE.changePushToken(token);
            });
    }
}