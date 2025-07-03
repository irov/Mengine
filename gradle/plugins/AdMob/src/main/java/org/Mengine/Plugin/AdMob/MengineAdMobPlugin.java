package org.Mengine.Plugin.AdMob;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.VersionInfo;

public class MengineAdMobPlugin extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "AdMob";

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        VersionInfo admobSdkVersion = MobileAds.getVersion();

        String admobSdkVersionString = String.valueOf(admobSdkVersion);

        this.logInfo("AdMob SDK version: " + admobSdkVersionString);
    }
}
