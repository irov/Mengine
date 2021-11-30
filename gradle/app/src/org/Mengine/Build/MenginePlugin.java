package org.Mengine.Build;

import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.KeyEvent;

public class MenginePlugin {
    private MengineActivity m_activity;

    public MengineActivity getActivity()
    {
        return m_activity;
    }

    public boolean onInitialize(MengineActivity activity)
    {
        m_activity = activity;

        return true;
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