package org.Mengine.Base;

import android.view.KeyEvent;

import androidx.annotation.NonNull;

public interface MengineListenerKeyEvent extends MengineServiceInterface {
    boolean onMengineKeyEvent(@NonNull MengineApplication application, @NonNull MengineActivity activity, KeyEvent event);
}