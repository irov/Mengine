package org.Mengine.Base;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;
import android.util.Log;

import java.util.ArrayList;

public class MengineActivityLifecycle implements Application.ActivityLifecycleCallbacks {
    public static final String TAG = "Lifecycle";

    public MengineApplication m_application;
    public MengineActivity m_activity;

    public MengineActivityLifecycle(MengineApplication application, MengineActivity activity) {
        m_application = application;
        m_activity = activity;
    }

    @Override
    public void onActivityResumed(Activity activity) {
        Log.d(TAG, "onActivityResumed");

        ArrayList<MenginePluginActivityLifecycleListener> listeners = m_application.getActivityLifecycleListeners();

        for (MenginePluginActivityLifecycleListener l : listeners) {
            l.onMengineActivityLifecycleResumed(activity);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleResumed");
    }

    @Override
    public void onActivityPaused(Activity activity) {
        Log.d(TAG, "onActivityPaused");

        ArrayList<MenginePluginActivityLifecycleListener> listeners = m_application.getActivityLifecycleListeners();

        for (MenginePluginActivityLifecycleListener l : listeners) {
            l.onMengineActivityLifecyclePaused(activity);
        }

        m_activity.pythonCall(TAG, "onActivityLifecyclePaused");
    }

    @Override
    public void onActivityStarted(Activity activity) {
        Log.d(TAG, "onActivityStarted");

        ArrayList<MenginePluginActivityLifecycleListener> listeners = m_application.getActivityLifecycleListeners();

        for (MenginePluginActivityLifecycleListener l : listeners) {
            l.onMengineActivityLifecycleStarted(activity);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleStarted");
    }

    @Override
    public void onActivityStopped(Activity activity) {
        Log.d(TAG, "onActivityStopped");

        ArrayList<MenginePluginActivityLifecycleListener> listeners = m_application.getActivityLifecycleListeners();

        for (MenginePluginActivityLifecycleListener l : listeners) {
            l.onMengineActivityLifecycleStopped(activity);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleStopped");
    }

    @Override
    public void onActivityDestroyed(Activity activity) {
        Log.d(TAG, "onActivityDestroyed");

        ArrayList<MenginePluginActivityLifecycleListener> listeners = m_application.getActivityLifecycleListeners();

        for (MenginePluginActivityLifecycleListener l : listeners) {
            l.onMengineActivityLifecycleDestroyed(activity);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleDestroyed");
    }

    @Override
    public void onActivityCreated(Activity activity, Bundle bundle) {
        Log.d(TAG, "onActivityCreated");

        ArrayList<MenginePluginActivityLifecycleListener> listeners = m_application.getActivityLifecycleListeners();

        for (MenginePluginActivityLifecycleListener l : listeners) {
            l.onMengineActivityLifecycleCreated(activity, bundle);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleCreated");
    }

    @Override
    public void onActivitySaveInstanceState(Activity activity, Bundle bundle) {
        Log.d(TAG, "onActivitySaveInstanceState");

        ArrayList<MenginePluginActivityLifecycleListener> listeners = m_application.getActivityLifecycleListeners();

        for (MenginePluginActivityLifecycleListener l : listeners) {
            l.onMengineActivityLifecycleSaveInstanceState(activity, bundle);
        }

        m_activity.pythonCall(TAG, "onActivityLifecycleSaveInstanceState");
    }
}