package org.Mengine.Plugin.FirebaseCrashlytics;

import android.os.Handler;
import android.os.Looper;

import com.google.firebase.crashlytics.CustomKeysAndValues;
import com.google.firebase.crashlytics.FirebaseCrashlytics;

import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineTag;

import java.util.Map;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

public class MengineFirebaseCrashlyticsANRMonitor {
    private static final MengineTag TAG = MengineTag.of("MengineFBCANRMonitor");
    private static final long TIMEOUT_MS = 10000;

    private static final Handler m_mainHandler = new Handler(Looper.getMainLooper());
    private static final Semaphore m_semaphore = new Semaphore(0);
    private static Thread m_watcherThread;
    private static volatile boolean m_anrDetected = false;

    private static final Runnable MAIN_CHECK_RUNNABLE = new Runnable() {
        @Override
        public void run() {
            m_semaphore.release();
        }
    };

    public static void start() {
        m_watcherThread = new Thread(() -> {
            while (Thread.currentThread().isInterrupted() == false) {
                m_mainHandler.post(MAIN_CHECK_RUNNABLE);

                try {
                    if (m_semaphore.tryAcquire(TIMEOUT_MS, TimeUnit.MILLISECONDS) == true) {
                        m_anrDetected = false;
                        continue;
                    }
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }

                if (m_anrDetected == true) {
                    continue;
                }

                m_anrDetected = true;

                Thread mainThread = Looper.getMainLooper().getThread();
                StackTraceElement[] mainStack = mainThread.getStackTrace();
                Map<Thread, StackTraceElement[]> allThreads = Thread.getAllStackTraces();

                if (BuildConfig.DEBUG == true) {
                    StringBuilder builder = new StringBuilder();
                    builder.append("Main thread is unresponsive for ")
                        .append(TIMEOUT_MS)
                        .append(" ms. This may indicate an ANR (Application Not Responding) issue.\n");
                    builder.append("Main thread stack trace (state: ").append(mainThread.getState()).append(")\n");
                    for (StackTraceElement ste : mainStack) {
                        builder.append("\tat ").append(ste.toString()).append("\n");
                    }
                    builder.append("All threads stack traces:\n");
                    for (Map.Entry<Thread, StackTraceElement[]> entry : allThreads.entrySet()) {
                        Thread thread = entry.getKey();

                        if (thread == mainThread) {
                            continue;
                        }

                        StackTraceElement[] threadStack = entry.getValue();

                        builder.append("Thread: ").append(thread.getName()).append(" (state: ").append(thread.getState()).append(")\n");
                        for (StackTraceElement ste : threadStack) {
                            builder.append("\tat ").append(ste.toString()).append("\n");
                        }
                    }
                    builder.append("Please check your main thread operations and ensure they are not blocking for too long.");
                    String errorMessage = builder.toString();
                    MengineLog.logError(MengineFirebaseCrashlyticsANRMonitor.TAG, errorMessage);
                }

                FirebaseCrashlytics crashlytics = FirebaseCrashlytics.getInstance();
                Throwable anrThrowable = new Throwable("Mengine ANR detected on main thread: " + mainThread.getName() + " (state: " + mainThread.getState() + ")");
                anrThrowable.setStackTrace(mainStack);

                for (Map.Entry<Thread, StackTraceElement[]> entry : allThreads.entrySet()) {
                    Thread thread = entry.getKey();

                    if (thread == mainThread) {
                        continue;
                    }

                    StackTraceElement[] stack = entry.getValue();

                    Throwable threadThrowable = new Throwable("Thread: " + thread.getName() + " (state: " + thread.getState() + ")");
                    threadThrowable.setStackTrace(stack);

                    anrThrowable.addSuppressed(threadThrowable);
                }

                CustomKeysAndValues params = new CustomKeysAndValues.Builder()
                    .putString("MainThreadName", mainThread.getName())
                    .putString("MainThreadState", mainThread.getState().toString())
                    .build();

                crashlytics.recordException(anrThrowable, params);
            }
        }, "MengineANRMonitor");

        m_watcherThread.start();
    }

    public static void stop() {
        if (m_watcherThread != null) {
            m_watcherThread.interrupt();
            m_watcherThread = null;
        }

        m_mainHandler.removeCallbacks(MAIN_CHECK_RUNNABLE);

        m_anrDetected = false;
    }
}
