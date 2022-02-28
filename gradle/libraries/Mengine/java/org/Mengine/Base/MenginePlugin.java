package org.Mengine.Base;

import android.content.Intent;
import android.content.Context;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;

import java.util.Formatter;

public class MenginePlugin {
    private MengineApplication m_application;
    private MengineActivity m_activity;
    private Formatter m_formatter;
    private String m_tag;
    private String m_pluginName;

    @FunctionalInterface
    public interface CallbackInterface{
        void callback();
    }

    public MengineApplication getApplication() { return m_application; }
    public MengineActivity getActivity()
    {
        return m_activity;
    }

    public boolean onInitialize(MengineApplication application)
    {
        m_application = application;

        StringBuilder sb = new StringBuilder();
        Formatter formatter = new Formatter(sb);

        m_formatter = formatter;

        m_tag = this.getClass().getSimpleName();

        return true;
    }

    public void setActivity(MengineActivity activity)
    {
        m_activity = activity;
    }

    protected void addPythonPlugin(String name)
    {
        m_pluginName = name;

        m_activity.addPythonPlugin(m_pluginName, this);
    }

    public void log(String format, Object ... args)
    {
        Formatter formatter = m_formatter.format(format, args);
        String message = formatter.toString();

        Log.i(m_tag, message);
    }

    public void pythonCall(String method, Object ... args)
    {
        m_activity.pythonCall(m_pluginName, method, args);
    }

    public void pythonCallCb(String method, CallbackInterface cb, Object ... args)
    {
        //m_activity.pythonCallCb(m_pluginName, method, cb, args);
    }

    public void onAppCreate(MengineApplication application)
    {
        //Empty
    }

    public void onAppTerminate(MengineApplication application)
    {
        //Empty
    }

    public void onAppAttachBaseContext(MengineApplication application, Context base)
    {
        //Empty
    }

    public void onAppConfigurationChanged(MengineApplication application, Configuration newConfig)
    {
        //Empty
    }

    public void onCreate(MengineActivity activity, Bundle savedInstanceState)
    {
        //Empty
    }

    public void onMengineInitializeBaseServices(MengineActivity activity)
    {
        //Empty
    }

    public void onMengineCreateApplication(MengineActivity activity)
    {
        //Empty
    }

    public void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data)
    {
        //Empty
    }

    public void onStart(MengineActivity activity)
    {
        //Empty
    }

    public void onStop(MengineActivity activity)
    {
        //Empty
    }

    public void onPause(MengineActivity activity)
    {
        //Empty
    }

    public void onResume(MengineActivity activity)
    {
        //Empty
    }

    public void onNewIntent(MengineActivity activity, Intent intent)
    {
        //Empty
    }

    public void onDestroy(MengineActivity activity)
    {
        //Empty
    }

    public void onRestart(MengineActivity activity)
    {
        //Empty
    }

    public void onConfigurationChanged(MengineActivity activity,Configuration newConfig)
    {
        //Empty
    }

    public void onRequestPermissionsResult(MengineActivity activity, int requestCode, String[] permissions, int[] grantResults)
    {
        //Empty
    }

    public boolean dispatchKeyEvent(MengineActivity activity, KeyEvent event)
    {
        //Empty

        return false;
    }

    public void onPythonEmbedding(MengineActivity activity)
    {
        //Empty
    }
}