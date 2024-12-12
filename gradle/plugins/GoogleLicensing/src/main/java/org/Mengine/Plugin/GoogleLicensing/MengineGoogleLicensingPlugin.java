package org.Mengine.Plugin.GoogleLicensing;

import android.os.Bundle;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import com.google.android.vending.licensing.LicenseChecker;
import com.google.android.vending.licensing.LicenseCheckerCallback;

public class MengineGoogleLicensingPlugin extends MenginePlugin implements MenginePluginActivityListener {
    public static final S tring SERVICE_NAME = "MengineGLicensing";

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        byte[] SALT = new byte[] {
                -46, 74, 53, -55, -12, 84, -24, 34, -12, -76, 12, 43, 22, 14, 11, 7, 54, 64, 18
        };

        LicenseChecker mChecker = new LicenseChecker(
                this, new ServerManagedPolicy(this, new AESObfuscator(SALT, getPackageName(), deviceId)),
                BASE64_PUBLIC_KEY
        );
    }
}