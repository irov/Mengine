package org.Mengine.Plugin.Firebase;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;

import com.google.firebase.FirebaseApp;

public class MengineFirebasePlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "Firebase";

    @Override
    public void onAppCreate(MengineApplication application) {
        FirebaseApp.initializeApp(application);
    }
}
