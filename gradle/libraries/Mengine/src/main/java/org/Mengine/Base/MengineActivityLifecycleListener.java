package org.Mengine.Base;

import android.app.Activity;
import android.os.Bundle;

public interface MengineActivityLifecycleListener {
    public void onMengineActivityLifecycleResumed(Activity activity);
    public void onMengineActivityLifecyclePaused(Activity activity);
    public void onMengineActivityLifecycleStarted(Activity activity);
    public void onMengineActivityLifecycleStopped(Activity activity);
    public void onMengineActivityLifecycleDestroyed(Activity activity);
    public void onMengineActivityLifecycleCreated(Activity activity, Bundle bundle);
    public void onMengineActivityLifecycleSaveInstanceState(Activity activity, Bundle bundle);
}