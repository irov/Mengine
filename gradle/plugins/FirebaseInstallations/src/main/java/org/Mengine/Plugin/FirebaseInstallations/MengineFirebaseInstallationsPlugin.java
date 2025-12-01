package org.Mengine.Plugin.FirebaseInstallations;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFragmentRemoteConfig;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import com.google.firebase.installations.FirebaseInstallations;

public class MengineFirebaseInstallationsPlugin extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "FBInstallations";

    @Override
    public void onAppInit(@NonNull MengineApplication application, boolean isMainProcess) throws MengineServiceInvalidInitializeException {
        FirebaseInstallations.getInstance().getId()
            .addOnCompleteListener(task -> {
                if (task.isSuccessful() == false) {
                    Exception exception = task.getException();

                    this.logMessage("Failed to get Firebase Installations ID: %s", exception.getMessage());

                    return;
                }

                String fid = task.getResult();

                this.logMessage("Firebase Installations ID: %s", fid);

                MengineFragmentRemoteConfig.INSTANCE.setInstallationId(fid);
            });
    }
}