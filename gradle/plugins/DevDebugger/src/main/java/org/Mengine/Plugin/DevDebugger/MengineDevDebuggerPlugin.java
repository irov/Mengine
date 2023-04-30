package org.Mengine.Plugin.DevDebugger;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import android.os.Bundle;

public class MengineDevDebuggerPlugin extends MenginePlugin {
    public static final String PLUGIN_NAME = "DevDebugger";

    @FunctionalInterface
    public interface DevButtonCallbackInterface {
        void callback();
    }

    private static native boolean AndroidDevDebuggerMengine_createDevTab(String tab);
    private static native void AndroidDevDebuggerMengine_addDevButton(String tab, String tag, String title, DevButtonCallbackInterface cb);

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
    }

    @Override
    public void onDestroy(MengineActivity activity) {
    }

    void createDevTab(String tab) {
        AndroidDevDebuggerMengine_createDevTab(tab);
    }

    void addDevButton(String tab, String tag, String title, DevButtonCallbackInterface cb) {
        AndroidDevDebuggerMengine_addDevButton(tab, tag, title, cb);
    }
}
