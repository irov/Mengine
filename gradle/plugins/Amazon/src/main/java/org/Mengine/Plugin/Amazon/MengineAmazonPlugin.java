package org.Mengine.Plugin.Amazon;

import android.content.Context;
import android.content.res.Configuration;

import com.amazon.device.ads.AdRegistration;
import com.amazon.device.ads.DTBAdNetwork;
import com.amazon.device.ads.DTBAdNetworkInfo;
import com.amazon.device.ads.MRAIDPolicy;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineAmazonPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "Amazon";

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String MengineAmazonPlugin_AppId = application.getMetaDataString("mengine.amazon.appId");

        try {
            AdRegistration.getInstance(MengineAmazonPlugin_AppId, application);
        } catch (IllegalArgumentException e) {
            this.invalidInitialize("AdRegistration get instance failed: %s"
                , e.getLocalizedMessage()
            );

            return;
        }

        AdRegistration.setAdNetworkInfo(new DTBAdNetworkInfo(DTBAdNetwork.MAX));
        AdRegistration.setMRAIDSupportedVersions(new String[]{"1.0", "2.0", "3.0"});
        AdRegistration.setMRAIDPolicy(MRAIDPolicy.CUSTOM);

        boolean MengineAmazonPlugin_EnableTesting = application.getMetaDataBoolean("mengine.amazon.enable_testing", false);

        if (MengineAmazonPlugin_EnableTesting == true) {
            AdRegistration.enableTesting(true);
        }

        boolean MengineAmazonPlugin_EnableLogging = application.getMetaDataBoolean("mengine.amazon.enable_logging", BuildConfig.DEBUG);

        if (MengineAmazonPlugin_EnableLogging == true) {
            AdRegistration.enableLogging(true);
        }
    }
}
