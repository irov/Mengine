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

    public final static int LM_SILENT = 0;
    public final static int LM_FATAL = 1;
    public final static int LM_CRITICAL = 2;
    public final static int LM_MESSAGE_RELEASE = 3;
    public final static int LM_ERROR = 4;
    public final static int LM_WARNING = 5;
    public final static int LM_MESSAGE = 6;
    public final static int LM_INFO = 7;
    public final static int LM_DEBUG = 8;
    public final static int LM_VERBOSE = 9;

    private static MengineApplication m_application;
    private static boolean m_initializeBaseServices = false;
    private static final Object m_lock = new Object();

    public static void setMengineApplication(MengineApplication application) {
        MengineLog.m_application = application;
    }

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

    public static void log(int level, String tag, String format, Object ... args) {
        switch (level) {
            case LM_SILENT:
                break;
            case LM_FATAL:
            case LM_CRITICAL:
                //ToDo
                break;
            case LM_MESSAGE_RELEASE:
                MengineLog.logMessageRelease(tag, format, args);
                break;
            case LM_ERROR:
                MengineLog.logError(tag, format, args);
                break;
            case LM_WARNING:
                MengineLog.logWarning(tag, format, args);
                break;
            case LM_MESSAGE:
                MengineLog.logMessage(tag, format, args);
                break;
            case LM_INFO:
                MengineLog.logInfo(tag, format, args);
                break;
            case LM_DEBUG:
                //ToDo
                break;
            case LM_VERBOSE:
                //ToDo
                break;
        }
    }

    public static void logInfo(String tag, String format, Object ... args) {
        String totalMsg = MengineLog.logBuildTotalMsg(tag, format, args);

        if( MengineLog.m_initializeBaseServices == true ) {
            synchronized (MengineLog.m_lock) {
                AndroidEnvironmentService_logInfo(totalMsg);
            }
        } else {
            Log.i(tag, totalMsg);

            if (MengineLog.m_application != null) {
                MengineLog.m_application.onMengineLogger(tag, LM_INFO, 0, 0, totalMsg);
            }
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

            if (MengineLog.m_application != null) {
                MengineLog.m_application.onMengineLogger(tag, LM_MESSAGE, 0, 0, totalMsg);
            }
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

            if (MengineLog.m_application != null) {
                MengineLog.m_application.onMengineLogger(tag, LM_WARNING, 0, 0, totalMsg);
            }
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

            if (MengineLog.m_application != null) {
                MengineLog.m_application.onMengineLogger(tag, LM_MESSAGE_RELEASE, 0, 0, totalMsg);
            }
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

            if (MengineLog.m_application != null) {
                MengineLog.m_application.onMengineLogger(tag, LM_ERROR, 0, 0, totalMsg);
            }
        }
    }
}
