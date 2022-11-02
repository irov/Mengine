package org.Mengine.Base;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineActivity;

import android.app.Activity;
import android.content.Intent;
import android.content.Context;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;

import java.util.ArrayList;

public class MenginePlugin {
    private MengineApplication m_application;
    private MengineActivity m_activity;
    private ArrayList<MenginePluginExtension> m_extensions;
    private String m_pluginName;

    @FunctionalInterface
    public interface CallbackInterface {
        void callback(Object result);
    }

    public MengineApplication getApplication() {
        return m_application;
    }

    public MengineActivity getActivity() {
        return m_activity;
    }

    public boolean onInitialize(MengineApplication application, String pluginName) {
        m_application = application;

        m_pluginName = pluginName;

        m_extensions = new ArrayList<>();

        return true;
    }

    public void onFinalize(MengineApplication application) {
        for( MenginePluginExtension extension : m_extensions ) {
            extension.onFinalize(m_activity, this);
        }

        m_extensions = null;
    }

    public void setActivity(MengineActivity activity) {
        m_activity = activity;
    }

    public String getPluginName() {
        return m_pluginName;
    }

    public <T> T newInstance( String name, boolean exist ) {
        T instance = MengineUtils.newInstance(m_pluginName, name, exist);

        return instance;
    }

    public void logWarning(String format, Object ... args) {
        MengineLog.logWarning(m_pluginName, format, args);
    }

    public void logInfo(String format, Object ... args) {
        MengineLog.logInfo(m_pluginName, format, args);
    }

    public void logError(String format, Object ... args) {
        MengineLog.logError(m_pluginName, format, args);
    }

    public void pythonCall(String method, Object ... args) {
        if(m_pluginName == null) {
            this.logError("invalid python method '%s' call before embedding", method);

            return;
        }

        m_activity.pythonCall(m_pluginName, method, args);
    }

    public void pythonCallCb(String method, CallbackInterface cb, Object ... args) {
        if(m_pluginName == null) {
            this.logError("invalid python method '%s' call before embedding", method);

            return;
        }

        m_activity.pythonCallCb(m_pluginName, method, cb, args);
    }

    public void onAppCreate(MengineApplication application) {
        //Empty
    }

    public void onAppTerminate(MengineApplication application) {
        //Empty
    }

    public void onAppAttachBaseContext(MengineApplication application, Context base) {
        //Empty
    }

    public void onAppConfigurationChanged(MengineApplication application, Configuration newConfig) {
        //Empty
    }

    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        //Empty
    }

    public void addExtension(String type) {
        MenginePluginExtension extension = this.newInstance(type, false);

        if (extension == null) {
            return;
        }

        if( extension.onInitialize(m_activity, this) == false) {
            return;
        }

        m_extensions.add(extension);
    }

    public void onExtension(MengineActivity activity) {
        //Empty
    }

    public void onExtensionRun(MengineActivity activity) {
        for( MenginePluginExtension extension : m_extensions ) {
            extension.onRun(activity, this);
        }
    }

    public void onMengineInitializeBaseServices(MengineActivity activity) {
        //Empty
    }

    public void onMengineCreateApplication(MengineActivity activity) {
        //Empty
    }

    public void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        //Empty
    }

    public void onStart(MengineActivity activity) {
        //Empty
    }

    public void onStop(MengineActivity activity) {
        //Empty
    }

    public void onPause(MengineActivity activity) {
        //Empty
    }

    public void onResume(MengineActivity activity) {
        //Empty
    }

    public void onNewIntent(MengineActivity activity, Intent intent) {
        //Empty
    }

    public void onDestroy(MengineActivity activity) {
        //Empty
    }

    public void onRestart(MengineActivity activity) {
        //Empty
    }

    public void onConfigurationChanged(MengineActivity activity,Configuration newConfig) {
        //Empty
    }

    public void onRequestPermissionsResult(MengineActivity activity, int requestCode, String[] permissions, int[] grantResults) {
        //Empty
    }

    public boolean dispatchKeyEvent(MengineActivity activity, KeyEvent event) {
        //Empty

        return false;
    }

    public void onActivityResumed(Activity activity) {
        //Empty
    }

    public void onActivityPaused(Activity activity) {
        //Empty
    }

    public void onActivityStarted(Activity activity) {
        //Empty
    }

    public void onActivityStopped(Activity activity) {
        //Empty
    }

    public void onActivityDestroyed(Activity activity) {
        //Empty
    }

    public void onActivityCreated(Activity activity, Bundle bundle) {
        //Empty
    }

    public void onActivitySaveInstanceState(Activity activity, Bundle bundle) {
        //Empty
    }
}