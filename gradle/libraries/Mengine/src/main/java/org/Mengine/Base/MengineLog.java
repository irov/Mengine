package org.Mengine.Base;

import android.util.Log;

import java.util.Formatter;
import java.util.IllegalFormatException;

public class MengineLog {
    private static native void AndroidEnvironmentService_logInfo(String msg);
    private static native void AndroidEnvironmentService_logWarning(String msg);
    private static native void AndroidEnvironmentService_logMessage(String msg);
    private static native void AndroidEnvironmentService_logMessageRelease(String msg);
    private static native void AndroidEnvironmentService_logError(String msg);

    private static boolean m_initializeBaseServices = false;
    private static final Object m_lock = new Object();

    public static void onMengineInitializeBaseServices(MengineActivity activity) {
        MengineLog.m_initializeBaseServices = true;
    }

    public static void onMengineApplicationStop(MengineActivity activity) {
        MengineLog.m_initializeBaseServices = false;
    }

    private static String logBuildTotalMsg(String tag, String format, Object ... args) {
        StringBuilder sb = new StringBuilder();
        Formatter formatter = new Formatter(sb);
        String msg;

        try {
            msg = formatter.format(format, args).toString();
        } catch (IllegalFormatException e) {
            String error = "Catch illegal format '" + format + "' exception: " + e.getLocalizedMessage();

            return error;
        }

        if( tag == null) {
            return msg;
        }

        String totalMsg = "[" + tag + "] " + msg;

        return totalMsg;
    }

    public static void logInfo(String tag, String format, Object ... args) {
        String totalMsg = MengineLog.logBuildTotalMsg(tag, format, args);

        if( MengineLog.m_initializeBaseServices == true ) {
            synchronized (MengineLog.m_lock) {
                AndroidEnvironmentService_logInfo(totalMsg);
            }
        } else {
            Log.i(tag, totalMsg);
        }
    }

    public static void logWarning(String tag, String format, Object ... args) {
        String totalMsg = MengineLog.logBuildTotalMsg(tag, format, args);

        if( MengineLog.m_initializeBaseServices == true ) {
            synchronized (MengineLog.m_lock) {
                AndroidEnvironmentService_logWarning(totalMsg);
            }
        } else {
            Log.w(tag, totalMsg);
        }
    }

    public static void logMessage(String tag, String format, Object ... args) {
        String totalMsg = MengineLog.logBuildTotalMsg(tag, format, args);

        if( MengineLog.m_initializeBaseServices == true ) {
            synchronized (MengineLog.m_lock) {
                AndroidEnvironmentService_logMessage(totalMsg);
            }
        } else {
            Log.w(tag, totalMsg);
        }
    }

    public static void logMessageRelease(String tag, String format, Object ... args) {
        String totalMsg = MengineLog.logBuildTotalMsg(tag, format, args);

        if( MengineLog.m_initializeBaseServices == true ) {
            synchronized (MengineLog.m_lock) {
                AndroidEnvironmentService_logMessageRelease(totalMsg);
            }
        } else {
            Log.e(tag, totalMsg);
        }
    }

    public static void logError(String tag, String format, Object ... args) {
        String totalMsg = MengineLog.logBuildTotalMsg(tag, format, args);

        if( MengineLog.m_initializeBaseServices == true ) {
            synchronized (MengineLog.m_lock) {
                AndroidEnvironmentService_logError(totalMsg);
            }
        } else {
            Log.e(tag, totalMsg);
        }
    }
}
