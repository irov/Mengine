package org.Mengine.Base;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;
import android.util.Log;

import java.util.ArrayList;

public class MengineActivityLifecycle implements Application.ActivityLifecycleCallbacks {
    public static final String TAG = "MengineActivityLifecycle";

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
            p.onActivityLifecycleResumed(activity);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleResumed");
    }

    @Override
    public void onActivityPaused(Activity activity) {
        Log.d(TAG, "onActivityPaused");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityLifecyclePaused(activity);
        }

        m_activity.pythonCall(TAG, "onActivityLifecyclePaused");
    }

    @Override
    public void onActivityStarted(Activity activity) {
        Log.d(TAG, "onActivityStarted");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityLifecycleStarted(activity);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleStarted");
    }

    @Override
    public void onActivityStopped(Activity activity) {
        Log.d(TAG, "onActivityStopped");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityLifecycleStopped(activity);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleStopped");
    }

    @Override
    public void onActivityDestroyed(Activity activity) {
        Log.d(TAG, "onActivityDestroyed");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityLifecycleDestroyed(activity);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleDestroyed");
    }

    @Override
    public void onActivityCreated(Activity activity, Bundle bundle) {
        Log.d(TAG, "onActivityCreated");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityLifecycleCreated(activity, bundle);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleCreated");
    }
    @Override
    public void onActivitySaveInstanceState(Activity activity, Bundle bundle) {
        Log.d(TAG, "onActivitySaveInstanceState");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityLifecycleSaveInstanceState(activity, bundle);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleSaveInstanceState");
    }
}