package org.Mengine.Base;

import android.app.Application;
import android.content.Context;
import android.content.res.Configuration;
import android.util.Log;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class MengineApplication extends Application {
    public static final String TAG = "MengineApplication";

    public ArrayList<MenginePlugin> m_plugins;
    public Map<String, MenginePlugin> m_dictionaryPlugins;

    public ArrayList<MengineLoggerListener> m_loggerListeners;
    public ArrayList<MengineAnalyticsListener> m_analyticsListeners;
    public ArrayList<MengineActivityLifecycleListener> m_activityLifecycleListeners;
    public ArrayList<MengineKeyListener> m_keyListeners;

    public MengineActivityLifecycle m_activityLifecycle;

    public MengineApplication() {
        m_plugins = new ArrayList<>();
        m_dictionaryPlugins = new HashMap<>();

        m_loggerListeners = new ArrayList<>();
        m_analyticsListeners = new ArrayList<>();
        m_activityLifecycleListeners = new ArrayList<>();
        m_keyListeners = new ArrayList<>();

        for (String n : this.getGradleAndroidPlugins()) {
            this.createPlugin(n);
        }
    }

    public String[] getGradleAndroidPlugins() {
        String[] empty = {};

        return empty;
    }

    public ArrayList<MenginePlugin> getPlugins() {
        return m_plugins;
    }

    public ArrayList<MengineLoggerListener> getLoggerListeners() {
        return m_loggerListeners;
    }

    public ArrayList<MengineAnalyticsListener> getAnalyticsListeners() {
        return m_analyticsListeners;
    }

    public ArrayList<MengineActivityLifecycleListener> getActivityLifecycleListeners() {
        return m_activityLifecycleListeners;
    }

    public ArrayList<MengineKeyListener> getKeyListeners() {
        return m_keyListeners;
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
            Log.e(TAG, "plugin not found field PLUGIN_NAME: " + cls.getName());

            return null;
        } catch (IllegalAccessException ex) {
            Log.e(TAG, "plugin invalid field PLUGIN_NAME: " + cls.getName());

            return null;
        }

        MenginePlugin plugin = this.findPlugin(name);

        return (T)plugin;
    }

    protected boolean createPlugin(String type) {
        ClassLoader cl = MengineActivity.class.getClassLoader();

        MenginePlugin plugin = MengineUtils.newInstance(TAG, type, true);

        if (plugin == null) {
            Log.e(TAG, "MengineApplication not found plugin: " + type);

            return false;
        }

        Class<?> cls = plugin.getClass();

        String name;

        try {
            Field PLUGIN_NAME = cls.getField("PLUGIN_NAME");

            name = (String)PLUGIN_NAME.get(null);
        } catch (NoSuchFieldException ex) {
            Log.e(TAG, "MengineApplication plugin not found field PLUGIN_NAME: " + type);

            return false;
        } catch (IllegalAccessException ex) {
            Log.e(TAG, "MengineApplication plugin invalid field PLUGIN_NAME: " + type);

            return false;
        }

        if (plugin.onInitialize(this, name) == false) {
            return false;
        }

        m_plugins.add(plugin);
        m_dictionaryPlugins.put(name, plugin);

        if (plugin instanceof MengineAnalyticsListener) {
            MengineAnalyticsListener listener = (MengineAnalyticsListener)plugin;

            m_analyticsListeners.add(listener);
        }

        if (plugin instanceof MengineLoggerListener) {
            MengineLoggerListener listener = (MengineLoggerListener)plugin;

            m_loggerListeners.add(listener);
        }

        if (plugin instanceof MengineActivityLifecycleListener) {
            MengineActivityLifecycleListener listener = (MengineActivityLifecycleListener)plugin;

            m_activityLifecycleListeners.add(listener);
        }

        if (plugin instanceof MengineKeyListener) {
            MengineKeyListener listener = (MengineKeyListener)plugin;

            m_keyListeners.add(listener);
        }

        Log.i(TAG, "MengineApplication add plugin: " + type + " [" + name + "]");

        return true;
    }

    public void onMengineInitializeBaseServices(MengineActivity activity) {
        m_activityLifecycle = new MengineActivityLifecycle(this, activity);
        this.registerActivityLifecycleCallbacks(m_activityLifecycle);
    }

    public void onMengineCreateApplication(MengineActivity activity) {
        Log.i(TAG, "onMengineCreateApplication");

        //Empty
    }

    public void onMengineApplicationRun(MengineActivity activity) {
        Log.i(TAG, "onMengineApplicationRun");

        //Empty
    }

    public void onMengineApplicationReady(MengineActivity activity) {
        Log.i(TAG, "onMengineApplicationReady");

        //Empty
    }

    public void onMengineApplicationStop(MengineActivity activity) {
        Log.i(TAG, "onMengineApplicationStop");

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

        Log.i(TAG, "onCreate");

        for (MenginePlugin p : m_plugins) {
            p.onAppCreate(this);
        }
    }

    @Override
    public void onTerminate() {
        super.onTerminate();

        Log.i(TAG, "onTerminate");

        for (MenginePlugin p : m_plugins) {
            p.onAppTerminate(this);
        }

        m_plugins = null;
        m_dictionaryPlugins = null;

        m_loggerListeners = null;
        m_analyticsListeners = null;
        m_activityLifecycleListeners = null;
        m_keyListeners = null;
    }

    @Override
    public void attachBaseContext(Context base) {
        super.attachBaseContext(base);

        Log.i(TAG, "attachBaseContext");

        for (MenginePlugin p : m_plugins) {
            p.onAppAttachBaseContext(this, base);
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        Log.i(TAG, "onConfigurationChanged");

        for (MenginePlugin p : m_plugins) {
            p.onAppConfigurationChanged(this, newConfig);
        }
    }
}