package org.Mengine.Plugin.FirebaseMessaging;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.messaging.FirebaseMessaging;
import com.google.firebase.messaging.RemoteMessage;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdRevenueParam;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginExtension;
import org.Mengine.Base.MenginePluginExtensionListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MengineRemoteMessageParam;

import android.content.Context;
import android.os.Bundle;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class MengineFirebaseMessagingPlugin extends MenginePlugin implements MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "FirebaseMessaging";
    public static final boolean PLUGIN_EMBEDDING = true;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();

        Context context = activity.getApplicationContext();
        int resultCode = apiAvailability.isGooglePlayServicesAvailable(context);

        if (resultCode == ConnectionResult.SUCCESS) {
            FirebaseMessaging.getInstance().getToken()
                .addOnCompleteListener(new OnCompleteListener<String>() {
                    @Override
                    public void onComplete(@NonNull Task<String> task) {
                        if (task.isSuccessful() == false) {
                            Exception exception = task.getException();

                            MengineFirebaseMessagingPlugin.this.logError("[ERROR] fetching FCM registration token failed: %s"
                                , exception.getMessage()
                            );

                            return;
                        }

                        // Get new FCM registration token
                        String token = task.getResult();

                        MengineFirebaseMessagingPlugin.this.logInfo("fetching FCM registration token successful: %s"
                            , token
                        );

                        MengineFirebaseMessagingPlugin.this.sendEvent(MengineEvent.EVENT_PUSH_TOKEN, token);
                    }
                });
        }
    }
}