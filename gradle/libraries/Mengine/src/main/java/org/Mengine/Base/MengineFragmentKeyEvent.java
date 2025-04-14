package org.Mengine.Base;

import android.view.KeyEvent;

import androidx.annotation.NonNull;

public class MengineFragmentKeyEvent extends MengineFragment<MengineListenerKeyEvent> {
    public static MengineFragmentKeyEvent INSTANCE = null;

    MengineFragmentKeyEvent() {
        super(MengineListenerKeyEvent.class);

        INSTANCE = this;
    }

    public boolean dispatchKeyEvent(@NonNull KeyEvent event) {
        if (this.propagateB(MengineListenerKeyEvent::onMengineKeyEvent, event) == true) {
            return true;
        }

        return false;
    }
}