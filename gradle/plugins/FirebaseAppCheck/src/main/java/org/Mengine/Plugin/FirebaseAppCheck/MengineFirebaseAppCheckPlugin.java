package org.Mengine.Plugin.FirebaseAppCheck;

import com.google.firebase.appcheck.FirebaseAppCheck;
import com.google.firebase.appcheck.playintegrity.PlayIntegrityAppCheckProviderFactory;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineFirebaseAppCheckPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "MengineFBAppCheck";
    public static final boolean PLUGIN_EMBEDDING = true;
    
    FirebaseAppCheck m_firebaseAppCheck;

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        FirebaseAppCheck firebaseAppCheck = FirebaseAppCheck.getInstance();

        PlayIntegrityAppCheckProviderFactory factory = PlayIntegrityAppCheckProviderFactory.getInstance();
        firebaseAppCheck.installAppCheckProviderFactory(factory);

        m_firebaseAppCheck = firebaseAppCheck;
    }
}