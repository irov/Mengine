package org.Mengine.Base;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineUtils;

import android.app.Application;
import android.content.Context;
import android.content.res.Configuration;
import android.util.Log;

import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;

public class MengineApplication extends Application {
    public static final String TAG = "MengineApplication";

    public ArrayList<MenginePlugin> m_plugins;

    public MengineApplication() {
        this.m_plugins = new ArrayList<MenginePlugin>();

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

    protected boolean createPlugin(String name) {
        ClassLoader cl = MengineActivity.class.getClassLoader();

        MenginePlugin plugin = MengineUtils.newInstance(TAG, name);

        if (plugin == null) {
            Log.e(TAG, "MengineApplication not found plugin: " + name);

            return false;
        }

        if (plugin.onInitialize(this) == false) {
            return false;
        }

        m_plugins.add(plugin);

        Log.i(TAG, "MengineApplication add plugin: " + name);

        return true;
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
            p.onAppTerminate(this);
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