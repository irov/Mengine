package org.Mengine.Plugin.AdMob;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerApplication;

import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.VersionInfo;

public class MengineAdMobPlugin extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "AdMob";

    @Override
    public String onAppVersion(@NonNull MengineApplication application) {
        VersionInfo admobSdkVersion = MobileAds.getVersion();

        String admobSdkVersionString = String.valueOf(admobSdkVersion);

        return admobSdkVersionString;
    }
}
