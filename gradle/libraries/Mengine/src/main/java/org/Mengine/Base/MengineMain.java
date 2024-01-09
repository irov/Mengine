package org.Mengine.Base;

import android.util.Log;

import org.libsdl.app.SDLActivity;

class MengineMain implements Runnable {
    public static final String TAG = "MengineMain";

    protected String library;
    protected String[] arguments;

    MengineMain(String library, String [] arguments) {
        this.library = library;
        this.arguments = arguments;
    }

    @Override
    public void run() {
        try {
            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_DISPLAY);
        } catch (Exception e) {
            Log.v(TAG, "modify thread properties failed " + e.toString());
        }

        String function = "SDL_main";

        Log.v(TAG, "Running main function " + function + " from library " + library);

        SDLActivity.nativeRunMain(library, function, arguments);

        Log.v(TAG, "Finished main function");

        /*
        if (SDLActivity.mSingleton != null && !SDLActivity.mSingleton.isFinishing()) {
            // Let's finish the Activity
            SDLActivity.mSDLThread = null;
            SDLActivity.mSingleton.finish();
        }  // else: Activity is already being destroyed
         */
    }
}