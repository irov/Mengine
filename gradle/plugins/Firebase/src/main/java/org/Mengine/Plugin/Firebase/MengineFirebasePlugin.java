package org.Mengine.Plugin.Firebase;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import com.google.firebase.FirebaseApp;

public class MengineFirebasePlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "MengineFB";

    @Override
    public void onAppInit(MengineApplication application, boolean isMainProcess) throws MenginePluginInvalidInitializeException {
        FirebaseApp firebaseApp = FirebaseApp.initializeApp(application);

        if (firebaseApp == null) {
            this.invalidInitialize("failed to initialize FirebaseApp");
        }
    }
}
