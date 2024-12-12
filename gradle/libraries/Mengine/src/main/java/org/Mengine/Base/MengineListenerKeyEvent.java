package org.Mengine.Base;

import android.view.KeyEvent;

public interface MengineListenerKeyEvent extends MengineServiceInterface {
    boolean dispatchKeyEvent(MengineActivity activity, KeyEvent event);
}