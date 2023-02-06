package org.Mengine.Base;

import android.app.Application;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;
import android.content.res.Configuration;
import android.os.Bundle;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class MengineApplication extends Application {
    public static final String TAG = "MengineApplication";

    public ArrayList<MenginePlugin> m_plugins;
    public Map<String, MenginePlugin> m_dictionaryPlugins;

    public ArrayList<MenginePluginLoggerListener> m_loggerListeners;
    public ArrayList<MenginePluginAnalyticsListener> m_analyticsListeners;
    public ArrayList<MenginePluginActivityLifecycleListener> m_activityLifecycleListeners;
    public ArrayList<MenginePluginKeyListener> m_keyListeners;
    public ArrayList<MenginePluginApplicationListener> m_applicationListeners;

    public MengineActivityLifecycle m_activityLifecycle;

    public MengineApplication() {
        m_plugins = new ArrayList<>();
        m_dictionaryPlugins = new HashMap<>();

        m_loggerListeners = new ArrayList<>();
        m_analyticsListeners = new ArrayList<>();
        m_activityLifecycleListeners = new ArrayList<>();
        m_keyListeners = new ArrayList<>();
        m_applicationListeners = new ArrayList<>();

        for (String n : this.getGradleAndroidPlugins()) {
            this.createPlugin(n);
        }
    }

    public String[] getGradleAndroidPlugins() {
        String[] empty = {};

        return empty;
    }

    @SuppressWarnings("deprecation")
    private ApplicationInfo getPackageApplicationInfo(PackageManager packageManager, String packageName) throws PackageManager.NameNotFoundException {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.TIRAMISU) {
            PackageManager.ApplicationInfoFlags flags = PackageManager.ApplicationInfoFlags.of(PackageManager.GET_META_DATA);

            return packageManager.getApplicationInfo(packageName, flags);
        } else {
            return packageManager.getApplicationInfo(packageName, PackageManager.GET_META_DATA);
        }
    }

    public Bundle getMetaDataBundle() {
        Context context = this.getApplicationContext();
        String packageName = context.getPackageName();
        PackageManager packageManager = context.getPackageManager();

        try {
            ApplicationInfo ai = this.getPackageApplicationInfo(packageManager, packageName);

            Bundle bundle = ai.metaData;

            return bundle;
        } catch (PackageManager.NameNotFoundException e) {
            MengineLog.logError(TAG, "Unable to load meta-data: %s"
                    , e.getLocalizedMessage()
            );
        }

        return null;
    }

    public String getMetaDataString(String name) {
        Bundle bundle = this.getMetaDataBundle();

        if (bundle == null) {
            return null;
        }

        String value = bundle.getString(name);

        return value;
    }

    public boolean getMetaDataBoolean(String name, boolean d) {
        Bundle bundle = this.getMetaDataBundle();

        if (bundle == null) {
            return d;
        }

        boolean value = bundle.getBoolean(name, d);

        return value;
    }

    public int getMetaDataInteger(String name, int d) {
        Bundle bundle = this.getMetaDataBundle();

        if (bundle == null) {
            return d;
        }

        int value = bundle.getInt(name, d);

        return value;
    }

    public ArrayList<MenginePlugin> getPlugins() {
        return m_plugins;
    }

    public ArrayList<MenginePluginLoggerListener> getLoggerListeners() {
        return m_loggerListeners;
    }

    public ArrayList<MenginePluginAnalyticsListener> getAnalyticsListeners() {
        return m_analyticsListeners;
    }

    public ArrayList<MenginePluginActivityLifecycleListener> getActivityLifecycleListeners() {
        return m_activityLifecycleListeners;
    }

    public ArrayList<MenginePluginKeyListener> getKeyListeners() {
        return m_keyListeners;
    }

    public ArrayList<MenginePluginApplicationListener> getApplicationListeners()
    {
        return m_applicationListeners;
    }

    public MenginePlugin findPlugin(String name) {
        MenginePlugin plugin = m_dictionaryPlugins.get(name);

        return plugin;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    @SuppressWarnings("unchecked")
    public <T> T getPlugin(Class<T> cls) {
        String name;

        try {
            Field PLUGIN_NAME = cls.getField("PLUGIN_NAME");

            name = (String)PLUGIN_NAME.get(null);
        } catch (NoSuchFieldException ex) {
            MengineLog.logError(TAG, "plugin not found field PLUGIN_NAME: %s"
                , cls.getName()
            );

            return null;
        } catch (IllegalAccessException ex) {
            MengineLog.logError(TAG, "plugin invalid field PLUGIN_NAME: %s"
                , cls.getName()
            );

            return null;
        }

        MenginePlugin plugin = this.findPlugin(name);

        return (T)plugin;
    }

    protected boolean createPlugin(String type) {
        ClassLoader cl = MengineActivity.class.getClassLoader();

        MenginePlugin plugin = MengineUtils.newInstance(TAG, type, true);

        if (plugin == null) {
            MengineLog.logError(TAG, "not found plugin: %s"
                , type
            );

            return false;
        }

        Class<?> cls = plugin.getClass();

        String name;

        try {
            Field PLUGIN_NAME = cls.getField("PLUGIN_NAME");

            name = (String)PLUGIN_NAME.get(null);
        } catch (NoSuchFieldException ex) {
            MengineLog.logError(TAG, "plugin not found field PLUGIN_NAME: %s"
                , type
            );

            return false;
        } catch (IllegalAccessException ex) {
            MengineLog.logError(TAG, "plugin invalid field PLUGIN_NAME: %s"
                , type
            );

            return false;
        }

        if (plugin.onInitialize(this, name) == false) {
            return false;
        }

        m_plugins.add(plugin);
        m_dictionaryPlugins.put(name, plugin);

        if (plugin instanceof MenginePluginAnalyticsListener) {
            MenginePluginAnalyticsListener listener = (MenginePluginAnalyticsListener)plugin;

            m_analyticsListeners.add(listener);
        }

        if (plugin instanceof MenginePluginLoggerListener) {
            MenginePluginLoggerListener listener = (MenginePluginLoggerListener)plugin;

            m_loggerListeners.add(listener);
        }

        if (plugin instanceof MenginePluginActivityLifecycleListener) {
            MenginePluginActivityLifecycleListener listener = (MenginePluginActivityLifecycleListener)plugin;

            m_activityLifecycleListeners.add(listener);
        }

        if (plugin instanceof MenginePluginKeyListener) {
            MenginePluginKeyListener listener = (MenginePluginKeyListener)plugin;

            m_keyListeners.add(listener);
        }

        if (plugin instanceof MenginePluginApplicationListener) {
            MenginePluginApplicationListener listener = (MenginePluginApplicationListener)plugin;

            m_applicationListeners.add(listener);
        }

        MengineLog.logInfo(TAG, "add plugin: %s [%s]"
            , type
            , name
        );

        return true;
    }

    public void onMengineInitializeBaseServices(MengineActivity activity) {
        m_activityLifecycle = new MengineActivityLifecycle(this, activity);

        this.registerActivityLifecycleCallbacks(m_activityLifecycle);
    }

    public void onMengineCreateApplication(MengineActivity activity) {
        MengineLog.logInfo(TAG, "onMengineCreateApplication");

        //Empty
    }

    public void onMengineApplicationRun(MengineActivity activity) {
        MengineLog.logInfo(TAG, "onMengineApplicationRun");

        //Empty
    }

    public void onMengineApplicationReady(MengineActivity activity) {
        MengineLog.logInfo(TAG, "onMengineApplicationReady");

        //Empty
    }

    public void onMengineApplicationStop(MengineActivity activity) {
        MengineLog.logInfo(TAG, "onMengineApplicationStop");

        for (MenginePlugin p : m_plugins) {
            p.onFinalize(this);
        }

        if (m_activityLifecycle != null) {
            this.unregisterActivityLifecycleCallbacks(m_activityLifecycle);
            m_activityLifecycle = null;
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();

        MengineLog.logInfo(TAG, "onCreate");

        for (MenginePluginApplicationListener l : m_applicationListeners) {
            l.onAppCreate(this);
        }
    }

    @Override
    public void onTerminate() {
        super.onTerminate();

        MengineLog.logInfo(TAG, "onTerminate");

        for (MenginePluginApplicationListener l : m_applicationListeners) {
            l.onAppTerminate(this);
        }

        m_plugins = null;
        m_dictionaryPlugins = null;

        m_loggerListeners = null;
        m_analyticsListeners = null;
        m_activityLifecycleListeners = null;
        m_keyListeners = null;
        m_applicationListeners = null;
    }

    @Override
    public void attachBaseContext(Context base) {
        super.attachBaseContext(base);

        MengineLog.logInfo(TAG, "attachBaseContext");

        for (MenginePluginApplicationListener l : m_applicationListeners) {
            l.onAppAttachBaseContext(this, base);
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        MengineLog.logInfo(TAG, "onConfigurationChanged");

        for (MenginePluginApplicationListener l : m_applicationListeners) {
            l.onAppConfigurationChanged(this, newConfig);
        }
    }
}