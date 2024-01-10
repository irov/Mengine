package org.Mengine.Base;

import android.util.Log;

import org.libsdl.app.SDLActivity;

class MengineMain implements Runnable {
    public static final String TAG = "MengineMain";

    protected String library;
    protected String[] arguments;

    private Object m_semaphoreInit;
    private Object m_semaphoreRun;

    MengineMain(Object semaphoreInit, Object semaphoreRun, String library, String [] arguments) {
        this.library = library;
        this.arguments = arguments;

        m_semaphoreInit = semaphoreInit;
        m_semaphoreRun = semaphoreRun;
    }

    @Override
    public void run() {
        try {
            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_DISPLAY);
        } catch (Exception e) {
            Log.v(TAG, "modify thread properties failed " + e.toString());
        }

        String function = "SDL_init";

        Log.v(TAG, "Running init main function [SDL_main] from library " + library);

        SDLActivity.nativeRunMain(library, "SDL_main", arguments);

        Log.v(TAG, "Finished init main function");

        synchronized (m_semaphoreInit) {
            m_semaphoreInit.notifyAll();
        }

        synchronized (m_semaphoreRun) {
            try {
                m_semaphoreRun.wait();
            } catch (InterruptedException e) {
                Log.e(TAG, "wait semaphore failed " + e.toString());

                return;
            }
        }

        Log.v(TAG, "Running quit main function [SDL_loop] from library " + library);

        SDLActivity.nativeLoopMain("SDL_loop");

        Log.v(TAG, "Finished quit main function");

        /*
        if (SDLActivity.mSingleton != null && !SDLActivity.mSingleton.isFinishing()) {
            // Let's finish the Activity
            SDLActivity.mSDLThread = null;
            SDLActivity.mSingleton.finish();
        }  // else: Activity is already being destroyed
         */
    }
}