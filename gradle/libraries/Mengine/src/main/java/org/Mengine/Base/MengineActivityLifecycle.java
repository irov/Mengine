package org.Mengine.Base;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;
import android.util.Log;

import java.util.ArrayList;

public class MengineActivityLifecycle implements Application.ActivityLifecycleCallbacks {
    public static final String TAG = "MengineLifecycle";

    public MengineApplication m_application;
    public MengineActivity m_activity;

    public MengineActivityLifecycle(MengineApplication application, MengineActivity activity) {
        m_application = application;
        m_activity = activity;
    }

    @Override
    public void onActivityResumed(Activity activity) {
        Log.d(TAG, "onActivityResumed");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityResumed(activity);
        }

        m_activity.pythonCall("MengineLifecycle", "onActivityResumed");
    }

    @Override
    public void onActivityPaused(Activity activity) {
        Log.d(TAG, "onActivityPaused");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityPaused(activity);
        }

        m_activity.pythonCall("MengineLifecycle", "onActivityPaused");
    }

    @Override
    public void onActivityStarted(Activity activity) {
        Log.d(TAG, "onActivityStarted");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityStarted(activity);
        }

        m_activity.pythonCall("MengineLifecycle", "onActivityStarted");
    }

    @Override
    public void onActivityStopped(Activity activity) {
        Log.d(TAG, "onActivityStopped");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityStopped(activity);
        }

        m_activity.pythonCall("MengineLifecycle", "onActivityStopped");
    }

    @Override
    public void onActivityDestroyed(Activity activity) {
        Log.d(TAG, "onActivityDestroyed");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityDestroyed(activity);
        }

        m_activity.pythonCall("MengineLifecycle", "onActivityDestroyed");
    }

    @Override
    public void onActivityCreated(Activity activity, Bundle bundle) {
        Log.d(TAG, "onActivityCreated");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityCreated(activity, bundle);
        }

        m_activity.pythonCall("MengineLifecycle", "onActivityCreated");
    }
    @Override
    public void onActivitySaveInstanceState(Activity activity, Bundle bundle) {
        Log.d(TAG, "onActivitySaveInstanceState");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivitySaveInstanceState(activity, bundle);
        }

        m_activity.pythonCall("MengineLifecycle", "onActivitySaveInstanceState");
    }
}