package org.Mengine.Plugin.Amazon;

import androidx.annotation.BoolRes;
import androidx.annotation.StringRes;

import com.amazon.device.ads.AdRegistration;
import com.amazon.device.ads.DTBAdNetwork;
import com.amazon.device.ads.DTBAdNetworkInfo;
import com.amazon.device.ads.MRAIDPolicy;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

public class MengineAmazonPlugin extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "Amazon";

    public static final @StringRes int METADATA_APP_ID = R.string.mengine_amazon_app_id;
    public static final @BoolRes int METADATA_ENABLE_TESTING = R.bool.mengine_amazon_enable_testing;
    public static final @BoolRes int METADATA_ENABLE_LOGGING = R.bool.mengine_amazon_enable_logging;

    @Override
    public void onAppCreate(MengineApplication application) throws MengineServiceInvalidInitializeException {
        String MengineAmazonPlugin_AppId = this.getResourceString(METADATA_APP_ID);

        this.logInfo("%s: %s"
            , this.getResourceName(METADATA_APP_ID)
            , MengineUtils.getRedactedValue(MengineAmazonPlugin_AppId)
        );

        try {
            AdRegistration.getInstance(MengineAmazonPlugin_AppId, application);
        } catch (final IllegalArgumentException e) {
            this.invalidInitialize("AdRegistration get instance failed: %s"
                , e.getMessage()
            );

            return;
        }

        AdRegistration.setMRAIDSupportedVersions(new String[]{"1.0", "2.0", "3.0"});
        AdRegistration.setMRAIDPolicy(MRAIDPolicy.CUSTOM);

        boolean MengineAmazonPlugin_EnableTesting = this.getResourceBoolean(METADATA_ENABLE_TESTING);

        if (MengineAmazonPlugin_EnableTesting == true) {
            AdRegistration.enableTesting(true);
        }

        this.logInfo("%s: %b"
            , this.getResourceName(METADATA_ENABLE_TESTING)
            , MengineAmazonPlugin_EnableTesting
        );

        boolean MengineAmazonPlugin_EnableLogging = this.getResourceBoolean(METADATA_ENABLE_LOGGING);

        if (MengineAmazonPlugin_EnableLogging == true) {
            AdRegistration.enableLogging(true);
        }

        this.logInfo("%s: %b"
            , this.getResourceName(METADATA_ENABLE_LOGGING)
            , MengineAmazonPlugin_EnableLogging
        );
    }
}
