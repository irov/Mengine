package org.Mengine.Base;

import android.app.Activity;
import android.os.Bundle;

public interface MenginePluginActivityLifecycleListener extends MenginePluginInterface {
    void onMengineActivityLifecycleResumed(Activity activity);
    void onMengineActivityLifecyclePaused(Activity activity);
    void onMengineActivityLifecycleStarted(Activity activity);
    void onMengineActivityLifecycleStopped(Activity activity);
    void onMengineActivityLifecycleDestroyed(Activity activity);
    void onMengineActivityLifecycleCreated(Activity activity, Bundle bundle);
    void onMengineActivityLifecycleSaveInstanceState(Activity activity, Bundle bundle);
}