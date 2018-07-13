package org.Mengine.Build;

import android.os.Handler;
import android.os.Looper;

/**
 * Created by sweatcoin7 on 7/12/18.
 */

public class ThreadUtil {
    public static void performOnMainThread(Runnable runnable) {
        new Handler(Looper.getMainLooper()).post(runnable);
    }
}
