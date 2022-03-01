package org.Mengine.Base;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineActivity;

import android.app.ActivityManager;
import android.app.Application;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.content.Context;
import android.content.res.Configuration;
import android.os.Process;
import android.util.Log;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.Iterator;

public class MengineApplication extends Application {
    public static final String TAG = "Mengine";

    public ArrayList<MenginePlugin> plugins;

    public MengineApplication() {
    }

    protected boolean createPlugin(String name) {
        ClassLoader cl = MengineActivity.class.getClassLoader();

        try {
            Class<?> clazz = cl.loadClass(name);
            Constructor<?> ctr = clazz.getConstructor();
            MenginePlugin plugin = (MenginePlugin) ctr.newInstance(new Object[]{});

            if (plugin.onInitialize(this) == false) {
                return false;
            }

            this.plugins.add(plugin);

            Log.i(TAG, "MengineApplication add plugin: " + name);

            return true;
        } catch (ClassNotFoundException ex) {
            Log.e(TAG, "MengineApplication not found plugin: " + name + " ClassNotFoundException: " + ex.toString());
        } catch (NoSuchMethodException ex) {
            Log.e(TAG, "MengineApplication not found plugin: " + name + " NoSuchMethodException: " + ex.toString());
        } catch (IllegalAccessException ex) {
            Log.e(TAG, "MengineApplication not found plugin: " + name + " IllegalAccessException: " + ex.toString());
        } catch (InstantiationException ex) {
            Log.e(TAG, "MengineApplication not found plugin: " + name + " InstantiationException: " + ex.toString());
        } catch (InvocationTargetException ex) {
            Log.e(TAG, "MengineApplication not found plugin: " + name + " InvocationTargetException: " + ex.toString());
        } catch (NullPointerException ex) {
            Log.e(TAG, "MengineApplication not found plugin: " + name + " NullPointerException: " + ex.toString());
        }

        return false;
    }

    @Override
    public void onCreate() {
        super.onCreate();

        for (MenginePlugin p : this.plugins) {
            p.onAppCreate(this);
        }
    }

    @Override
    public void onTerminate() {
        super.onTerminate();

        for (MenginePlugin p : this.plugins) {
            p.onAppTerminate(this);
        }
    }

  /*  @Override
    public void attachBaseContext(Context base) {
        super.attachBaseContext(base);

        for (MenginePlugin p : this.plugins) {
            p.onAppAttachBaseContext(this, base);
        }
    }
*/
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        for (MenginePlugin p : this.plugins) {
            p.onAppConfigurationChanged(this, newConfig);
        }
    }
}