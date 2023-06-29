package org.Mengine.Base;

import android.view.KeyEvent;

public interface MenginePluginKeyListener extends MenginePluginInterface {
    boolean dispatchKeyEvent(MengineActivity activity, KeyEvent event);
}