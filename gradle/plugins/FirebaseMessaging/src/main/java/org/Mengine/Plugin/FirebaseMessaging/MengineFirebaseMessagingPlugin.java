package org.Mengine.Plugin.FirebaseMessaging;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.firebase.messaging.FirebaseMessaging;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;

public class MengineFirebaseMessagingPlugin extends MengineService implements MengineListenerActivity {
    public static final String SERVICE_NAME = "FBMessaging";
    public static final boolean SERVICE_EMBEDDING = true;

    @Override
    public boolean onAvailable(@NonNull MengineApplication application) {
        GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();

        Context context = application.getApplicationContext();
        int resultCode = apiAvailability.isGooglePlayServicesAvailable(context);

        if (resultCode != ConnectionResult.SUCCESS) {
            return false;
        }

        return true;
    }

    public void askNotificationPermission(@NonNull MengineActivity activity) {
        ActivityResultLauncher<String> requestPermissionLauncher =
            activity.registerForActivityResult(new ActivityResultContracts.RequestPermission(), isGranted -> {
                if (isGranted) {
                    // FCM SDK (and your app) can post notifications.
                } else {
                    // TODO: Inform user that that your app will not show notifications.
                }
            });

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            if (ContextCompat.checkSelfPermission(activity, Manifest.permission.POST_NOTIFICATIONS) == PackageManager.PERMISSION_GRANTED) {
            } else if (activity.shouldShowRequestPermissionRationale(Manifest.permission.POST_NOTIFICATIONS)) {
                // TODO: display an educational UI explaining to the user the features that will be enabled
                //       by them granting the POST_NOTIFICATION permission. This UI should provide the user
                //       "OK" and "No thanks" buttons. If the user selects "OK," directly request the permission.
                //       If the user selects "No thanks," allow the user to continue without notifications.
            } else {
                requestPermissionLauncher.launch(Manifest.permission.POST_NOTIFICATIONS);
            }
        }
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        this.askNotificationPermission(activity);

        FirebaseMessaging firebaseMessaging = FirebaseMessaging.getInstance();

        firebaseMessaging.getToken()
            .addOnCompleteListener(task -> {
                if (task.isSuccessful() == false) {
                    Exception exception = task.getException();

                    this.logError("[ERROR] fetching FCM registration token failed: %s"
                        , exception.getMessage()
                    );

                    return;
                }

                // Get new FCM registration token
                String token = task.getResult();

                this.logInfo("fetching FCM registration token success: %s"
                    , token
                );

                MengineApplication application = activity.getMengineApplication();

                application.onMenginePushToken(token);
            });
    }
}