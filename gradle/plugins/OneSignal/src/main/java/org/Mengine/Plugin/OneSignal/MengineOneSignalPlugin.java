package org.Mengine.Plugin.OneSignal;

import com.onesignal.OneSignal;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;

public class MengineOneSignalPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "OneSignal";

    @Override
    public void onAppCreate(MengineApplication application) {

        OneSignal.setLogLevel(OneSignal.LOG_LEVEL.VERBOSE, OneSignal.LOG_LEVEL.NONE);

        OneSignal.initWithContext(application);

        String onesignal_app_id = application.getString(R.string.onesignal_app_id);

        OneSignal.setAppId(onesignal_app_id);
    }

    public void logError(String message) {
        OneSignal.onesignalLog(OneSignal.LOG_LEVEL.ERROR, message);
    }
}
