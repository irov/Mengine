package org.Mengine.Plugin.Firebase;

import android.content.Context;
import android.content.res.Configuration;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;

import com.google.firebase.FirebaseApp;

public class MengineFirebasePlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "Firebase";

    @Override
    public void onAppCreate(MengineApplication application) {
        FirebaseApp.initializeApp(application);
    }
}
