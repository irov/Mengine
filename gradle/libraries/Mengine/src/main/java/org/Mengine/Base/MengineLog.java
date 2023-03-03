package org.Mengine.Base;

import android.util.Log;

import java.util.Formatter;
import java.util.IllegalFormatException;

public class MengineLog {
    private static native void AndroidEnvironmentService_log(int level, String msg);

    public final static int LM_SILENT = 0;
    public final static int LM_FATAL = 1;
    public final static int LM_MESSAGE_RELEASE = 2;
    public final static int LM_ERROR = 3;
    public final static int LM_WARNING = 4;
    public final static int LM_MESSAGE = 5;
    public final static int LM_INFO = 6;
    public final static int LM_DEBUG = 7;
    public final static int LM_VERBOSE = 8;

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

    public static void log(int level, String tag, String format, Object ... args) {
        String totalMsg = MengineLog.buildTotalMsg(tag, format, args);

        if( MengineLog.m_initializeBaseServices == true ) {
            synchronized (MengineLog.m_lock) {
                AndroidEnvironmentService_log(level, totalMsg);
            }
        } else {
            switch (level) {
                case LM_SILENT:
                    return;
                case LM_FATAL:
                    Log.wtf(tag, totalMsg);
                    break;
                case LM_MESSAGE_RELEASE:
                    Log.e(tag, totalMsg);
                    break;
                case LM_ERROR:
                    Log.e(tag, totalMsg);
                    break;
                case LM_WARNING:
                    Log.w(tag, totalMsg);
                    break;
                case LM_INFO:
                    Log.i(tag, totalMsg);
                    break;
                case LM_DEBUG:
                    Log.d(tag, totalMsg);
                    break;
                case LM_VERBOSE:
                    Log.v(tag, totalMsg);
                    break;
            }

            if (MengineLog.m_application != null) {
                String msg = MengineLog.buildTotalMsg(null, format, args);

                MengineLog.m_application.onMengineLogger(tag, level, 0, 0, msg);
            }
        }
    }

    public static void logInfo(String tag, String format, Object ... args) {
        MengineLog.log(LM_INFO, tag, format, args);
    }

    public static void logMessage(String tag, String format, Object ... args) {
        MengineLog.log(LM_MESSAGE, tag, format, args);
    }

    public static void logWarning(String tag, String format, Object ... args) {
        MengineLog.log(LM_WARNING, tag, format, args);
    }

    public static void logFatal(String tag, String format, Object ... args) {
        MengineLog.log(LM_FATAL, tag, format, args);
    }

    public static void logMessageRelease(String tag, String format, Object ... args) {
        MengineLog.log(LM_MESSAGE_RELEASE, tag, format, args);
    }

    public static void logError(String tag, String format, Object ... args) {
        MengineLog.log(LM_ERROR, tag, format, args);
    }

    private static String buildTotalMsg(String tag, String format, Object ... args) {
        StringBuilder sb = new StringBuilder();
        Formatter formatter = new Formatter(sb);
        String msg;

        try {
            msg = formatter.format(format, args).toString();
        } catch (IllegalFormatException e) {
            String error = "Catch illegal format: '" + format + "' args: '" + args + "' exception: " + e.getLocalizedMessage();

            return error;
        }

        if (tag == null) {
            return msg;
        }

        String totalMsg = "[" + tag + "] " + msg;

        return totalMsg;
    }
}
