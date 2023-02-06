package org.Mengine.Base;

import android.view.KeyEvent;

public interface MenginePluginKeyListener {
    public boolean dispatchKeyEvent(MengineActivity activity, KeyEvent event);
}