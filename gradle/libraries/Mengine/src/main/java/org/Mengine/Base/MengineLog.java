package org.Mengine.Base;

import android.util.Log;

import java.util.Formatter;

public class MengineLog {
    private static native void AndroidNativeMengine_logInfo(String msg);
    private static native void AndroidNativeMengine_logWarning(String msg);
    private static native void AndroidNativeMengine_logMessage(String msg);
    private static native void AndroidNativeMengine_logMessageRelease(String msg);
    private static native void AndroidNativeMengine_logError(String msg);

    private static boolean initializeBaseServices = false;

    public static void onMengineInitializeBaseServices() {
        MengineLog.initializeBaseServices = true;
    }

    public static String logBuildTotalMsg(String tag, String format, Object ... args) {
        StringBuilder sb = new StringBuilder();
        Formatter formatter = new Formatter(sb);
        String msg = formatter.format(format, args).toString();

        if( tag == null) {
            return msg;
        }

        String totalMsg = "[" + tag + "] " + msg;

        return totalMsg;
    }

    public static void logInfo(String tag, String format, Object ... args) {
        if( MengineLog.initializeBaseServices == true ) {
            String totalMsg = MengineLog.logBuildTotalMsg(null, format, args);

            AndroidNativeMengine_logInfo(totalMsg);
        } else {
            String totalMsg = MengineLog.logBuildTotalMsg(null, format, args);

            Log.i(tag, totalMsg);
        }
    }

    public static void logWarning(String tag, String format, Object ... args) {
        if( MengineLog.initializeBaseServices == true ) {
            String totalMsg = MengineLog.logBuildTotalMsg(tag, format, args);

            AndroidNativeMengine_logWarning(totalMsg);
        } else {
            String totalMsg = MengineLog.logBuildTotalMsg(null, format, args);

            Log.w(tag, totalMsg);
        }
    }

    public static void logMessage(String tag, String format, Object ... args) {
        if( MengineLog.initializeBaseServices == true ) {
            String totalMsg = MengineLog.logBuildTotalMsg(tag, format, args);

            AndroidNativeMengine_logMessage(totalMsg);
        } else {
            String totalMsg = MengineLog.logBuildTotalMsg(null, format, args);

            Log.w(tag, totalMsg);
        }
    }

    public static void logMessageRelease(String tag, String format, Object ... args) {
        if( MengineLog.initializeBaseServices == true ) {
            String totalMsg = MengineLog.logBuildTotalMsg(tag, format, args);

            AndroidNativeMengine_logMessageRelease(totalMsg);
        } else {
            String totalMsg = MengineLog.logBuildTotalMsg(null, format, args);

            Log.w(tag, totalMsg);
        }
    }

    public static void logError(String tag, String format, Object ... args) {
        if( MengineLog.initializeBaseServices == true ) {
            String totalMsg = MengineLog.logBuildTotalMsg(tag, format, args);

            AndroidNativeMengine_logError(totalMsg);
        } else {
            String totalMsg = MengineLog.logBuildTotalMsg(null, format, args);

            Log.e(tag, totalMsg);
        }
    }
}
