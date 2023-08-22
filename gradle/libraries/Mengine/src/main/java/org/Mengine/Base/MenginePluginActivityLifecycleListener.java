package org.Mengine.Base;

import android.app.Activity;
import android.os.Bundle;

public interface MenginePluginActivityLifecycleListener extends MenginePluginInterface {
    default void onMengineActivityLifecycleResumed(Activity activity) {
        //Empty
    }

    default void onMengineActivityLifecyclePaused(Activity activity) {
        //Empty
    }

    default void onMengineActivityLifecycleStarted(Activity activity) {
        //Empty
    }

    default void onMengineActivityLifecycleStopped(Activity activity) {
        //Empty
    }

    default void onMengineActivityLifecycleDestroyed(Activity activity) {
        //Empty
    }

    default void onMengineActivityLifecycleCreated(Activity activity, Bundle bundle) {
        //Empty
    }

    default void onMengineActivityLifecycleSaveInstanceState(Activity activity, Bundle bundle) {
        //Empty
    }
}