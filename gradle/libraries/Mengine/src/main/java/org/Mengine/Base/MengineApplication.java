package org.Mengine.Base;

import android.app.Application;
import android.content.Context;
import android.content.res.Configuration;
import android.util.Log;

import androidx.lifecycle.ProcessLifecycleOwner;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class MengineApplication extends Application {
    public static final String TAG = "MengineApplication";

    public ArrayList<MenginePlugin> m_plugins;
    public Map<String, MenginePlugin> m_dictionaryPlugins;

    public MengineActivityLifecycle m_activityLifecycle;
    public MengineLifecycle m_lifecycle;

    public MengineComponentCallbacks m_componentCallbacks;

    public MengineApplication() {
        m_plugins = new ArrayList<MenginePlugin>();
        m_dictionaryPlugins = new HashMap<>();

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

    public MenginePlugin findPlugin(String name) {
        MenginePlugin plugin = m_dictionaryPlugins.get(name);

        return plugin;
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

        Log.i(TAG, "MengineApplication add plugin: " + type + " [" + name + "]");

        return true;
    }

    public void onMengineInitializeBaseServices(MengineActivity activity) {
        m_activityLifecycle = new MengineActivityLifecycle(this, activity);
        this.registerActivityLifecycleCallbacks(m_activityLifecycle);

        m_lifecycle = new MengineLifecycle(this, activity);
        ProcessLifecycleOwner.get().getLifecycle().addObserver(m_lifecycle);

        m_componentCallbacks = new MengineComponentCallbacks(this, activity);
        this.registerComponentCallbacks(m_componentCallbacks);
    }

    public void onMengineCreateApplication(MengineActivity activity) {
        //Empty
    }

    @Override
    public void onCreate() {
        super.onCreate();

        Log.i(TAG, "onCreate");

        for (MenginePlugin p : this.m_plugins) {
            p.onAppCreate(this);
        }
    }

    @Override
    public void onTerminate() {
        super.onTerminate();

        Log.i(TAG, "onTerminate");

        for (MenginePlugin p : this.m_plugins) {
            p.onFinalize(this);
        }

        for (MenginePlugin p : this.m_plugins) {
            p.onAppTerminate(this);
        }

        this.m_plugins = null;

        if (m_activityLifecycle != null) {
            this.unregisterActivityLifecycleCallbacks(m_activityLifecycle);
            m_activityLifecycle = null;
        }

        if (m_lifecycle != null) {
            ProcessLifecycleOwner.get().getLifecycle().removeObserver(m_lifecycle);
            m_lifecycle = null;
        }

        if (m_componentCallbacks != null) {
            this.unregisterComponentCallbacks(m_componentCallbacks);
            m_componentCallbacks = null;
        }
    }

    @Override
    public void attachBaseContext(Context base) {
        super.attachBaseContext(base);

        Log.i(TAG, "attachBaseContext");

        for (MenginePlugin p : this.m_plugins) {
            p.onAppAttachBaseContext(this, base);
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        Log.i(TAG, "onConfigurationChanged");

        for (MenginePlugin p : this.m_plugins) {
            p.onAppConfigurationChanged(this, newConfig);
        }
    }
}