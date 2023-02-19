package org.Mengine.Base;

import android.view.KeyEvent;

public interface MenginePluginKeyListener {
    boolean dispatchKeyEvent(MengineActivity activity, KeyEvent event);
}