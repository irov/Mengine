package org.Mengine.Plugin.GoogleService;

import android.content.Context;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;

public class MengineGoogleServicePlugin extends MenginePlugin {
    public static final String PLUGIN_NAME = "MengineGService";
    public static final boolean PLUGIN_EMBEDDING = false;

    private static int m_cacheIsGooglePlayServicesResultCode = ConnectionResult.UNKNOWN;

    public static boolean isGooglePlayServicesAvailable(MengineApplication application) {
        if (m_cacheIsGooglePlayServicesResultCode != ConnectionResult.UNKNOWN) {
            boolean available = m_cacheIsGooglePlayServicesResultCode == ConnectionResult.SUCCESS;

            return available;
        }

        GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();

        Context context = application.getApplicationContext();
        int resultCode = apiAvailability.isGooglePlayServicesAvailable(context);

        MengineGoogleServicePlugin.m_cacheIsGooglePlayServicesResultCode = resultCode;

        if (resultCode != ConnectionResult.SUCCESS) {
            return false;
        }

        return true;
    }
}
