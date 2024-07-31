package org.Mengine.Plugin.FirebaseMessaging;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.messaging.FirebaseMessaging;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import android.content.Context;
import android.os.Bundle;

import androidx.annotation.NonNull;

public class MengineFirebaseMessagingPlugin extends MenginePlugin implements MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "MengineFBMessaging";
    public static final boolean PLUGIN_EMBEDDING = true;

    @Override
    public boolean onAvailable(MengineApplication application) {
        GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();

        Context context = application.getApplicationContext();
        int resultCode = apiAvailability.isGooglePlayServicesAvailable(context);

        if (resultCode != ConnectionResult.SUCCESS) {
            return false;
        }

        return true;
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
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

                    MengineApplication application = activity.getMengineApplication();

                    application.onMenginePushToken(token);
                }
            });
    }
}