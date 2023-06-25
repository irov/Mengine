package org.Mengine.Base;

import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Bundle;

import java.lang.reflect.Field;
import java.util.ArrayList;

public class MenginePlugin {
    private MengineApplication m_application;
    private MengineActivity m_activity;
    private ArrayList<MenginePluginExtension> m_extensions = new ArrayList<>();
    private String m_pluginName;

    public MengineApplication getApplication() {
        return m_application;
    }

    public MengineActivity getActivity() {
        return m_activity;
    }

    public boolean onInitialize(MengineApplication application, String pluginName) {
        m_application = application;
        m_pluginName = pluginName;

        return true;
    }

    public void onFinalize(MengineApplication application) {
        m_application = null;
    }

    public void setActivity(MengineActivity activity) {
        m_activity = activity;
    }

    public String getPluginName() {
        return m_pluginName;
    }

    public <T> T newInstance(String name, boolean exist) {
        T instance = MengineUtils.newInstance(m_pluginName, name, exist);

        return instance;
    }

    public SharedPreferences getPrivateSharedPreferences() {
        SharedPreferences settings = m_application.getPrivateSharedPreferences(m_pluginName);

        return settings;
    }

    public void setPrivateSettingString(String name, String value) {
        SharedPreferences settings = this.getPrivateSharedPreferences();

        SharedPreferences.Editor editor = settings.edit();
        editor.putString(name, value);
        editor.apply();
    }

    public String getPrivateSettingString(String name, String defaultValue) {
        SharedPreferences settings = this.getPrivateSharedPreferences();

        String value = settings.getString(name, defaultValue);

        return value;
    }

    protected void invalidInitialize(String format, Object ... args) throws MenginePluginInvalidInitializeException {
        MengineLog.logFatal(m_pluginName, format, args);

        throw new MenginePluginInvalidInitializeException(m_pluginName);
    }

    public void setState(String name, Object value) {
        m_application.setState(name, value);
    }

    public String logInfo(String format, Object ... args) {
        return MengineLog.logInfo(m_pluginName, format, args);
    }

    public String logMessage(String format, Object ... args) {
        return MengineLog.logMessage(m_pluginName, format, args);
    }

    public String logWarning(String format, Object ... args) {
        return MengineLog.logWarning(m_pluginName, format, args);
    }

    public String logError(String format, Object ... args) {
        return MengineLog.logError(m_pluginName, format, args);
    }

    public void assertionError(String format, Object ... args) {
        String msg = String.format(format, args);

        throw new AssertionError("[" + m_pluginName + "] " + msg);
    }

    public MengineAnalyticsEventBuilder buildEvent(String name) {
        MengineAnalyticsEventBuilder eventBuilder = MengineAnalytics.buildEvent(name);

        return eventBuilder;
    }

    public void pythonCall(String method, Object ... args) {
        if (m_activity == null) {
            return;
        }

        m_activity.pythonCall(m_pluginName, method, args);
    }

    public void activateSemaphore(String name) {
        if (m_activity == null) {
            return;
        }

        m_activity.activateSemaphore(name);
    }

    public void waitSemaphore(String name, MengineFunctorVoid cb) {
        if (m_activity == null) {
            return;
        }

        m_activity.waitSemaphore(name, cb);
    }

    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        //Empty
    }

    public void onState(MengineApplication application, String name, Object value) {
        //Empty
    }

    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        //Empty
    }

    public boolean createExtension(MengineActivity activity, String type) {
        this.logMessage("plugin [%s] add extension: %s"
            , m_pluginName
            , type
        );

        MenginePluginExtension extension = m_application.createPluginExtension(activity, this, type);

        if (extension == null) {
            this.logError("plugin [%s] invalid create extension: %s"
                , m_pluginName
                , type
            );

            return false;
        }

        m_extensions.add(extension);

        return true;
    }

    public void onExtensionInitialize(MengineActivity activity) throws MenginePluginInvalidInitializeException {
        Class<?> c = this.getClass();
        Package p = c.getPackage();

        if (p == null) {
            return;
        }

        Class<?> buildConfig = MengineUtils.getPackageBuildConfig(m_pluginName, p);

        Field f;

        try {
            f = buildConfig.getField("MENGINE_GRADLE_ANDROID_PLUGIN_EXTENSIONS");
        } catch (NoSuchFieldException e) {
            return;
        } catch (NullPointerException e) {
            this.logError("plugin [%s] invalid get MENGINE_GRADLE_ANDROID_PLUGIN_EXTENSIONS exception: %s [NullPointerException]"
                , m_pluginName
                , e.getLocalizedMessage()
            );

            throw new MenginePluginInvalidInitializeException(m_pluginName);
        } catch (SecurityException e) {
            this.logError("plugin [%s] invalid get MENGINE_GRADLE_ANDROID_PLUGIN_EXTENSIONS exception: %s [SecurityException]"
                , m_pluginName
                , e.getLocalizedMessage()
            );

            throw new MenginePluginInvalidInitializeException(m_pluginName);
        }

        Object extension;

        try {
            extension = f.get(this);
        } catch (IllegalArgumentException e) {
            this.logError("plugin [%s] invalid get extension from this exception: %s [IllegalArgumentException]"
                , m_pluginName
                , e.getLocalizedMessage()
            );

            throw new MenginePluginInvalidInitializeException(m_pluginName);
        } catch (IllegalAccessException e) {
            this.logError("plugin [%s] invalid get extension from this exception: %s [IllegalAccessException]"
                , m_pluginName
                , e.getLocalizedMessage()
            );

            throw new MenginePluginInvalidInitializeException(m_pluginName);
        }

        if (extension == null) {
            return;
        }

        for (String extensionName : (String[])extension) {
            if (this.createExtension(activity, extensionName) == false) {
                throw new MenginePluginInvalidInitializeException(extensionName);
            }
        }
    }

    public void onExtensionRun(MengineActivity activity) {
        for (MenginePluginExtension extension : m_extensions) {
            extension.onPluginExtensionRun(activity, this);
        }
    }

    public void onExtensionFinalize(MengineActivity activity) {
        for (MenginePluginExtension extension : m_extensions) {
            extension.onPluginExtensionFinalize(activity, this);
        }

        m_extensions = null;
    }

    public void onMengineInitializeBaseServices(MengineActivity activity) {
        //Empty
    }

    public void onMengineCreateApplication(MengineActivity activity) {
        //Empty
    }

    public void onMenginePlatformRun(MengineActivity activity) {
        //Empty
    }

    public void onMenginePlatformReady(MengineActivity activity) {
        //Empty
    }

    public void onMenginePlatformStop(MengineActivity activity) {
        //Empty
    }

    public void onMengineCurrentSceneChange(MengineActivity activity, String name) {
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

    public void onConfigurationChanged(MengineActivity activity, Configuration newConfig) {
        //Empty
    }

    public void onRequestPermissionsResult(MengineActivity activity, int requestCode, String[] permissions, int[] grantResults) {
        //Empty
    }
}