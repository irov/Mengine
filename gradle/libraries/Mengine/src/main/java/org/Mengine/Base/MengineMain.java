package org.Mengine.Base;

import android.os.Process;

import java.util.concurrent.CountDownLatch;

public class MengineMain implements Runnable {
    public static final String TAG = "MengineMain";

    protected static CountDownLatch m_runLatch = new CountDownLatch(1);

    protected final Object m_nativeApplication;

    protected Thread m_thread;

    public MengineMain(Object nativeApplication) {
        m_nativeApplication = nativeApplication;
    }

    public static void runLatch() {
        m_runLatch.countDown();
    }

    public void start() {
        m_thread = new Thread(this);
        m_thread.start();
    }

    public void stop() {
        try {
            m_thread.join();
        } catch (final InterruptedException e) {
            MengineLog.logError(TAG, "thread main join exception: %s"
                , e.getMessage()
            );
        }
    }

    @Override
    public void run() {
        try {
            Thread.currentThread().setName("MengineMain");
        } catch (final Exception e) {
            MengineLog.logMessage(TAG, "modify main thread name exception: %s"
                , e.getMessage()
            );
        }

        try {
            Process.setThreadPriority(Process.THREAD_PRIORITY_DISPLAY);
        } catch (final Exception e) {
            MengineLog.logMessage(TAG, "modify main thread priority exception: %s"
                , e.getMessage()
            );
        }

        try {
            m_runLatch.await();
        } catch (final InterruptedException e) {
            MengineLog.logMessage(TAG, "wait runLatch exception: %s"
                , e.getMessage()
            );

            return;
        }

        if (MengineNative.AndroidMain_main(m_nativeApplication) == false) {
            MengineLog.logMessage(TAG, "main finish with failed" );

            return;
        }

        MengineLog.logMessage(TAG, "main finish succesful");
    }
}