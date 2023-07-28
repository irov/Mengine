package org.Mengine.Plugin.OneSignal;

import com.onesignal.OneSignal;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineOneSignalPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "OneSignal";

    public static final String PLUGIN_METADATA_APP_ID = "mengine.onesignal.app_id";

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String MengineOneSignalPlugin_AppId = application.getMetaDataString(PLUGIN_METADATA_APP_ID);

        if (MengineOneSignalPlugin_AppId == null) {
            this.invalidInitialize("invalid setup meta data [%s]", PLUGIN_METADATA_APP_ID);

            return;
        }

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_APP_ID
            , MengineOneSignalPlugin_AppId
        );

        OneSignal.setLogLevel(OneSignal.LOG_LEVEL.VERBOSE, OneSignal.LOG_LEVEL.NONE);
        OneSignal.initWithContext(application);
        OneSignal.setAppId(MengineOneSignalPlugin_AppId);
    }

    public void logError(String message) {
        OneSignal.onesignalLog(OneSignal.LOG_LEVEL.ERROR, message);
    }
}
