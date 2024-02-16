package org.Mengine.Base;

import android.app.Activity;
import android.util.Log;

import org.libsdl.app.SDLActivity;

class MengineMain implements Runnable {
    public static final String TAG = "MengineMain";

    Activity m_activity;

    protected String m_library;
    protected String[] m_arguments;

    private Object m_semaphoreInit;
    private Object m_semaphoreRun;

    private int m_statusInit;

    MengineMain(Activity activity, Object semaphoreInit, Object semaphoreRun, String library, String [] arguments) {
        m_activity = activity;

        m_semaphoreInit = semaphoreInit;
        m_semaphoreRun = semaphoreRun;

        m_library = library;
        m_arguments = arguments;

        m_statusInit = 0;
    }

    @Override
    public void run() {
        try {
            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_DISPLAY);
        } catch (Exception e) {
            MengineLog.logWarning(TAG, "modify thread properties failed: %s", e);
        }

        String function = "SDL_init";

        MengineLog.logInfo(TAG, "Running main function [SDL_main] from library: " + m_library);

        m_statusInit = SDLActivity.nativeRunMain(m_library, "SDL_main", m_arguments);

        MengineLog.logInfo(TAG, "Finished main function [SDL_main] status: %s", m_statusInit);

        synchronized (m_semaphoreInit) {
            m_semaphoreInit.notifyAll();
        }

        if(m_statusInit != 0) {
            return;
        }

        synchronized (m_semaphoreRun) {
            try {
                m_semaphoreRun.wait();
            } catch (InterruptedException e) {
                MengineLog.logError(TAG, "wait semaphore failed: %s", e);

                return;
            }
        }

        MengineLog.logInfo(TAG, "Running loop function [SDL_loop] from library: " + m_library);

        int statusRun = SDLActivity.nativeLoopMain("SDL_loop");

        MengineLog.logInfo(TAG, "Finished loop function [SDL_loop]");

        if (statusRun != 0) {
            MengineAnalytics.buildEvent("mng_activity_init_failed")
                .addParameterLong("run_status", statusRun)
                .logAndFlush();

            MengineUtils.finishActivityWithAlertDialog(m_activity, "[ERROR] SDL run failed with status: " + statusRun);
        }

        if (m_activity.isFinishing() == false) {
            m_activity.finish();
        }
    }

    public int getStatusInit() {
        return m_statusInit;
    }
}