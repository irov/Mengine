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

    public static final String PLUGIN_METADATA_APP_ID = "mengine.amazon.app_id";
    public static final String PLUGIN_METADATA_ENABLE_TESTING = "mengine.amazon.enable_testing";
    public static final String PLUGIN_METADATA_ENABLE_LOGGING = "mengine.amazon.enable_logging";

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String MengineAmazonPlugin_AppId = this.getMetaDataString(PLUGIN_METADATA_APP_ID);

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_APP_ID
            , MengineAmazonPlugin_AppId
        );

        try {
            AdRegistration.getInstance(MengineAmazonPlugin_AppId, application);
        } catch (IllegalArgumentException e) {
            this.invalidInitialize("AdRegistration get instance failed: %s"
                , MengineAmazonPlugin_AppId
                , e.getMessage()
            );

            return;
        }

        AdRegistration.setAdNetworkInfo(new DTBAdNetworkInfo(DTBAdNetwork.MAX));
        AdRegistration.setMRAIDSupportedVersions(new String[]{"1.0", "2.0", "3.0"});
        AdRegistration.setMRAIDPolicy(MRAIDPolicy.CUSTOM);

        boolean MengineAmazonPlugin_EnableTesting = this.getMetaDataBoolean(PLUGIN_METADATA_ENABLE_TESTING);

        if (MengineAmazonPlugin_EnableTesting == true) {
            AdRegistration.enableTesting(true);
        }

        this.logInfo("%s: %b"
            , PLUGIN_METADATA_ENABLE_TESTING
            , MengineAmazonPlugin_EnableTesting
        );

        boolean MengineAmazonPlugin_EnableLogging = this.getMetaDataBoolean(PLUGIN_METADATA_ENABLE_LOGGING);

        if (MengineAmazonPlugin_EnableLogging == true) {
            AdRegistration.enableLogging(true);
        }

        this.logInfo("%s: %b"
            , PLUGIN_METADATA_ENABLE_LOGGING
            , MengineAmazonPlugin_EnableLogging
        );
    }
}
