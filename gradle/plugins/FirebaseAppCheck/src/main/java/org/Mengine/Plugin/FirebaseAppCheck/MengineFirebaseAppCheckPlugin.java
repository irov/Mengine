package org.Mengine.Plugin.FirebaseAppCheck;

import com.google.firebase.appcheck.FirebaseAppCheck;
import com.google.firebase.appcheck.playintegrity.PlayIntegrityAppCheckProviderFactory;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

public class MengineFirebaseAppCheckPlugin extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "MengineFBAppCheck";
    public static final boolean SERVICE_EMBEDDING = true;
    
    FirebaseAppCheck m_firebaseAppCheck;

    @Override
    public void onAppPrepare(MengineApplication application) throws MengineServiceInvalidInitializeException {
        FirebaseAppCheck firebaseAppCheck = FirebaseAppCheck.getInstance();

        PlayIntegrityAppCheckProviderFactory factory = PlayIntegrityAppCheckProviderFactory.getInstance();
        firebaseAppCheck.installAppCheckProviderFactory(factory);

        m_firebaseAppCheck = firebaseAppCheck;
    }
}