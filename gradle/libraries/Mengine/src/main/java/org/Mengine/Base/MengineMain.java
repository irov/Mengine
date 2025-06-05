package org.Mengine.Base;

import android.os.Process;

import androidx.annotation.NonNull;

import java.util.concurrent.CountDownLatch;

public class MengineMain implements Runnable {
    public static final String TAG = "MengineMain";

    protected static CountDownLatch m_runLatch = new CountDownLatch(1);

    protected final Object m_nativeApplication;

    protected Thread m_thread;

    public MengineMain(@NonNull Object nativeApplication) {
        m_nativeApplication = nativeApplication;
    }

    public static void runLatch() {
        m_runLatch.countDown();
    }

    public void start() {
        MengineLog.logMessage(TAG, "main start");

        m_thread = new Thread(this, "MengineMain");
        m_thread.start();
    }

    public void stop() {
        MengineLog.logMessage(TAG, "main join");

        try {
            m_thread.join();
        } catch (final InterruptedException e) {
            MengineLog.logError(TAG, "thread main join exception: %s"
                , e.getMessage()
            );
        }

        MengineLog.logMessage(TAG, "main stopped");
    }

    @Override
    public void run() {
        MengineLog.logMessage(TAG, "main wait latch");

        try {
            m_runLatch.await();
        } catch (final InterruptedException e) {
            MengineLog.logMessage(TAG, "wait runLatch exception: %s"
                , e.getMessage()
            );

            return;
        }

        MengineLog.logMessage(TAG, "main run");

        try {
            Process.setThreadPriority(Process.THREAD_PRIORITY_DISPLAY);
        } catch (final Exception e) {
            MengineLog.logMessage(TAG, "modify main thread priority exception: %s"
                , e.getMessage()
            );
        }

        if (MengineNative.AndroidMain_main(m_nativeApplication) == false) {
            MengineLog.logMessage(TAG, "main finish with failed" );

            return;
        }

        MengineLog.logMessage(TAG, "main finish succesful");
    }
}