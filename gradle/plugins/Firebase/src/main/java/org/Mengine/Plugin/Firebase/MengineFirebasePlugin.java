package org.Mengine.Plugin.Firebase;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import com.google.firebase.FirebaseApp;

public class MengineFirebasePlugin extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "MengineFB";

    @Override
    public void onAppInit(@NonNull MengineApplication application, boolean isMainProcess) throws MengineServiceInvalidInitializeException {
        FirebaseApp firebaseApp = FirebaseApp.initializeApp(application);

        if (firebaseApp == null) {
            this.invalidInitialize("failed to initialize FirebaseApp");
        }
    }
}
