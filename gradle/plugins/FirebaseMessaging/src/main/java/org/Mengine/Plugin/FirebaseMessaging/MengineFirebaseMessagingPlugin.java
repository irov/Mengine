package org.Mengine.Plugin.FirebaseMessaging;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.messaging.FirebaseMessaging;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import android.content.Context;
import android.os.Bundle;

import androidx.annotation.NonNull;

public class MengineFirebaseMessagingPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "FirebaseMessaging";
    public static boolean PLUGIN_EMBEDDING = true;

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
}