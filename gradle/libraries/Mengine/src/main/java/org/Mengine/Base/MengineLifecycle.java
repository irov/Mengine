package org.Mengine.Base;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.lifecycle.DefaultLifecycleObserver;
import androidx.lifecycle.LifecycleOwner;

import java.util.ArrayList;

public class MengineLifecycle implements DefaultLifecycleObserver {
    public static final String TAG = "MengineLifecycle";

    public MengineApplication m_application;
    public MengineActivity m_activity;

    public MengineLifecycle(MengineApplication application, MengineActivity activity) {
        m_application = application;
        m_activity = activity;
    }

    @Override
    public void onCreate(@NonNull LifecycleOwner owner) {
        Log.d(TAG, "onCreate");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onLifecycleCreate(owner);
        }

        m_activity.pythonCall(TAG, "onLifecycleCreate");
    }

    @Override
    public void onStart(@NonNull LifecycleOwner owner) {
        Log.d(TAG, "onStart");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onLifecycleStart(owner);
        }

        m_activity.pythonCall(TAG, "onLifecycleStart");
    }

    @Override
    public void onResume(@NonNull LifecycleOwner owner) {
        Log.d(TAG, "onResume");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onLifecycleResume(owner);
        }

        m_activity.pythonCall(TAG, "onLifecycleResume");
    }

    @Override
    public void onPause(@NonNull LifecycleOwner owner) {
        Log.d(TAG, "onPause");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onLifecyclePause(owner);
        }

        m_activity.pythonCall(TAG, "onLifecyclePause");
    }

    @Override
    public void onStop(@NonNull LifecycleOwner owner) {
        Log.d(TAG, "onStop");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onLifecycleStop(owner);
        }

        m_activity.pythonCall(TAG, "onLifecycleStop");
    }

    @Override
    public void onDestroy(@NonNull LifecycleOwner owner) {
        Log.d(TAG, "onDestroy");

        ArrayList<MenginePlugin> plugins = m_application.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onLifecycleDestroy(owner);
        }

        m_activity.pythonCall(TAG, "onLifecycleDestroy");
    }
}