package org.Mengine.Plugin.GoogleService;

import android.content.Context;

import androidx.annotation.NonNull;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

public class MengineGoogleServicePlugin extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "GService";
    public static final boolean SERVICE_EMBEDDING = false;

    private static int m_cacheIsGooglePlayServicesResultCode = ConnectionResult.UNKNOWN;

    public static boolean isGooglePlayServicesAvailable(MengineApplication application) {
        if (MengineGoogleServicePlugin.m_cacheIsGooglePlayServicesResultCode == ConnectionResult.SUCCESS) {
            return true;
        }

        if (MengineGoogleServicePlugin.m_cacheIsGooglePlayServicesResultCode == ConnectionResult.UNKNOWN) {
            GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();

            Context context = application.getApplicationContext();
            int resultCode = apiAvailability.isGooglePlayServicesAvailable(context);

            MengineGoogleServicePlugin.m_cacheIsGooglePlayServicesResultCode = resultCode;
        }

        if (MengineGoogleServicePlugin.m_cacheIsGooglePlayServicesResultCode != ConnectionResult.SUCCESS) {
            return false;
        }

        return true;
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();

        Context context = application.getApplicationContext();
        int clientVersion = apiAvailability.getClientVersion(context);

        this.logInfo("Google Play Services client version: %s", String.valueOf(clientVersion));
    }
}