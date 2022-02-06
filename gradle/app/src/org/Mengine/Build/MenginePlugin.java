package org.Mengine.Build;

import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;

import java.util.Formatter;

public class MenginePlugin {
    private MengineActivity m_activity;
    private Formatter m_formatter;
    private String m_tag;
    private String m_pluginName;

    public MengineActivity getActivity()
    {
        return m_activity;
    }

    public boolean onInitialize(MengineActivity activity)
    {
        m_activity = activity;

        StringBuilder sb = new StringBuilder();
        Formatter formatter = new Formatter(sb);

        m_formatter = formatter;

        m_tag = this.getClass().getSimpleName();

        return true;
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

    public void onCreate(Bundle savedInstanceState)
    {
        //Empty
    }

    public void onMengineInitializeBaseServices()
    {
        //Empty
    }

    public void onMengineCreateApplication()
    {
        //Empty
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        //Empty
    }

    public void onStart()
    {
        //Empty
    }

    public void onStop()
    {
        //Empty
    }

    public void onPause()
    {
        //Empty
    }

    public void onResume()
    {
        //Empty
    }

    public void onNewIntent(Intent intent)
    {
        //Empty
    }

    public void onDestroy()
    {
        //Empty
    }

    public void onRestart()
    {
        //Empty
    }

    public void onConfigurationChanged(Configuration newConfig)
    {
        //Empty
    }

    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults)
    {
        //Empty
    }

    public boolean dispatchKeyEvent(KeyEvent event)
    {
        //Empty

        return false;
    }

    public void onPythonEmbedding()
    {
        //Empty
    }
}