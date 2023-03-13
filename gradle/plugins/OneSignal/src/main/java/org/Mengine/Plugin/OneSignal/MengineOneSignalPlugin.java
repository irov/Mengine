package org.Mengine.Plugin.OneSignal;

import android.content.Context;
import android.content.res.Configuration;

import com.onesignal.OneSignal;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;

public class MengineOneSignalPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "OneSignal";

    @Override
    public void onAppCreate(MengineApplication application) {
        OneSignal.setLogLevel(OneSignal.LOG_LEVEL.VERBOSE, OneSignal.LOG_LEVEL.NONE);

        OneSignal.initWithContext(application);

        String onesignal_app_id = application.getMetaDataString("mengine.onesignal.app_id");

        OneSignal.setAppId(onesignal_app_id);
    }

    public void logError(String message) {
        OneSignal.onesignalLog(OneSignal.LOG_LEVEL.ERROR, message);
    }
}
