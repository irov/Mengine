package org.Mengine.Base;

import android.app.Activity;
import android.app.Application;
import android.content.ComponentCallbacks2;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.NonNull;

import java.util.ArrayList;

public class MengineComponentCallbacks implements ComponentCallbacks2 {
    public static final String TAG = "ComponentCallbacks";

    public MengineApplication m_application;
    public MengineActivity m_activity;

    public MengineComponentCallbacks(MengineApplication application, MengineActivity activity) {
        m_application = application;
        m_activity = activity;
    }

    @Override
    public void onTrimMemory(int level) {
        Log.d(TAG, "onTrimMemory");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onTrimMemory(m_application, level);
        }

        m_activity.pythonCall(TAG, "onTrimMemory", level);
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        Log.d(TAG, "onConfigurationChanged");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onConfigurationChanged(m_application, newConfig);
        }

        m_activity.pythonCall(TAG, "onConfigurationChanged");
    }

    @Override
    public void onLowMemory() {
        Log.d(TAG, "onLowMemory");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onLowMemory(m_application);
        }

        m_activity.pythonCall(TAG, "onLowMemory");
    }
}