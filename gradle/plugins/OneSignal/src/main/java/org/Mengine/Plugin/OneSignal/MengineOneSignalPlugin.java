package org.Mengine.Plugin.OneSignal;

import com.onesignal.OneSignal;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineOneSignalPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String SERVICE_NAME = "MengineOneSignal";

    public static final String METADATA_APP_ID = "mengine.onesignal.app_id";

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String MengineOneSignalPlugin_AppId = this.getMetaDataString(METADATA_APP_ID);

        this.logInfo("%s: %s"
            , METADATA_APP_ID
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
