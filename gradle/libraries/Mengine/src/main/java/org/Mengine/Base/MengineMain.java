package org.Mengine.Base;

import android.os.Process;

import java.util.concurrent.CountDownLatch;

public class MengineMain implements Runnable {
    public static final String TAG = "MengineMain";

    private static native boolean AndroidMain_main(Object application);

    MengineActivity m_activity;

    Object m_nativeApplication;
    CountDownLatch m_runLatch;

    public MengineMain(MengineActivity activity, Object nativeApplication, CountDownLatch runLatch) {
        m_activity = activity;
        m_nativeApplication = nativeApplication;
        m_runLatch = runLatch;
    }

    @Override
    public void run() {
        try {
            Thread.currentThread().setName("MengineMain");
        } catch (Exception e) {
            MengineLog.logMessage(TAG, "modify main thread name exception: %s"
                , e.getMessage()
            );
        }

        try {
            Process.setThreadPriority(Process.THREAD_PRIORITY_DISPLAY);
        } catch (Exception e) {
            MengineLog.logMessage(TAG, "modify main thread priority exception: %s"
                , e.getMessage()
            );
        }

        try {
            m_runLatch.await();
        } catch (InterruptedException e) {
            MengineLog.logMessage(TAG, "wait runLatch exception: %s"
                , e.getMessage()
            );

            return;
        }

        if (AndroidMain_main(m_nativeApplication) == false) {
            MengineUtils.finishActivityWithAlertDialog(m_activity, "main finish with failed");

            return;
        }

        MengineLog.logMessage(TAG, "main finish succesful");
    }
}