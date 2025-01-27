package org.Mengine.Plugin.FirebaseAppCheck;

import androidx.annotation.NonNull;

import com.google.firebase.appcheck.AppCheckToken;
import com.google.firebase.appcheck.FirebaseAppCheck;
import com.google.firebase.appcheck.playintegrity.PlayIntegrityAppCheckProviderFactory;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import java.util.Map;

public class MengineFirebaseAppCheckPlugin extends MengineService implements MengineListenerApplication, FirebaseAppCheck.AppCheckListener {
    public static final String SERVICE_NAME = "FBAppCheck";
    public static final boolean SERVICE_EMBEDDING = true;
    
    FirebaseAppCheck m_firebaseAppCheck;

    @Override
    public void onAppPrepare(@NonNull MengineApplication application, @NonNull Map<String, String> pluginVersions) throws MengineServiceInvalidInitializeException {
        FirebaseAppCheck firebaseAppCheck = FirebaseAppCheck.getInstance();

        PlayIntegrityAppCheckProviderFactory factory = PlayIntegrityAppCheckProviderFactory.getInstance();
        firebaseAppCheck.installAppCheckProviderFactory(factory);

        m_firebaseAppCheck = firebaseAppCheck;
    }

    @Override
    public void onAppCheckTokenChanged(@NonNull AppCheckToken token) {
        // Send token to your backend to verify the token

        this.logInfo("AppCheck token changed: " + token.getToken());
    }
}