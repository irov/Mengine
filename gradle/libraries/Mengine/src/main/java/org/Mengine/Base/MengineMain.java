package org.Mengine.Base;

import android.os.Process;

import androidx.annotation.NonNull;

import java.util.concurrent.CountDownLatch;

public class MengineMain implements Runnable {
    public static final MengineTag TAG = MengineTag.of("MMain");

    protected static final CountDownLatch m_runLatch = new CountDownLatch(1);

    protected final Object m_nativeApplication;

    protected Thread m_thread;

    public MengineMain(@NonNull Object nativeApplication) {
        m_nativeApplication = nativeApplication;
    }

    public static void runLatch() {
        m_runLatch.countDown();
    }

    public void start() {
        MengineLog.logInfo(TAG, "main start");

        m_thread = new Thread(this, "MengineMain");
        m_thread.start();
    }

    public void stop() {
        MengineLog.logInfo(TAG, "main join");

        m_thread.interrupt();

        try {
            m_thread.join();
        } catch (final InterruptedException e) {
            MengineLog.logInfo(TAG, "main join interrupted: %s"
                , e.getMessage()
            );
        }

        m_thread = null;

        MengineLog.logInfo(TAG, "main stopped");
    }

    @Override
    public void run() {
        MengineLog.logInfo(TAG, "main wait latch");

        try {
            m_runLatch.await();
        } catch (final InterruptedException e) {
            Thread.currentThread().interrupt();
            return;
        }

        MengineLog.logInfo(TAG, "main run");

        try {
            Process.setThreadPriority(Process.THREAD_PRIORITY_DISPLAY);
        } catch (final Exception e) {
            MengineLog.logInfo(TAG, "modify main thread priority exception: %s"
                , e.getMessage()
            );
        }

        if (MengineNative.AndroidMain_main(m_nativeApplication) == false) {
            MengineLog.logInfo(TAG, "main finish with failed" );

            return;
        }

        MengineLog.logInfo(TAG, "main finish successful");
    }
}