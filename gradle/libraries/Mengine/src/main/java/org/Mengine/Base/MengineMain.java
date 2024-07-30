package org.Mengine.Base;

import android.os.Process;

public class MengineMain implements Runnable {
    public static final String TAG = "MengineMain";

    private static native void AndroidMain_nativeRunMain(Object activity);

    private String m_args;

    public MengineMain(String args) {
        m_args = args;
    }

    @Override
    public void run() {
        try {
            Process.setThreadPriority(Process.THREAD_PRIORITY_DISPLAY);
        } catch (Exception e) {
            MengineLog.logError(TAG, "modify main thread priority exception: "
                    , e.getMessage()
            );
        }

        AndroidMain_nativeRunMain(m_args);
    }
}