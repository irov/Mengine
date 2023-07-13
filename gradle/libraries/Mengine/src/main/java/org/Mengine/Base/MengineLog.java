package org.Mengine.Base;

import android.util.Log;

import java.util.Arrays;
import java.util.Formatter;
import java.util.IllegalFormatException;

public class MengineLog {
    private static native void AndroidEnvironmentService_log(int level, String tag, String msg);

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

    public static void onMenginePlatformStop(MengineActivity activity) {
        MengineLog.m_initializeBaseServices = false;
    }

    private static String log(int level, String tag, String format, Object ... args) {
        String totalMsg = MengineLog.buildTotalMsg(format, args);

        switch (level) {
            case LM_SILENT:
                return totalMsg;
            case LM_FATAL:
                Log.wtf(tag, totalMsg);
                break;
            case LM_MESSAGE_RELEASE:
                Log.w(tag, totalMsg);
                break;
            case LM_ERROR:
                Log.e(tag, totalMsg);
                break;
            case LM_WARNING:
                Log.w(tag, totalMsg);
                break;
            case LM_MESSAGE:
                Log.i(tag, totalMsg);
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

        if( MengineLog.m_initializeBaseServices == true ) {
            synchronized (MengineLog.m_lock) {
                AndroidEnvironmentService_log(level, tag, totalMsg);
            }
        } else {
            if (MengineLog.m_application != null) {
                MengineLog.m_application.onMengineLogger(tag, level, 0, 0, totalMsg);
            }
        }

        return totalMsg;
    }

    public static String logVerbose(String tag, String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        return MengineLog.log(LM_VERBOSE, tag, format, args);
    }

    public static String logDebug(String tag, String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        return MengineLog.log(LM_DEBUG, tag, format, args);
    }

    public static String logInfo(String tag, String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        return MengineLog.log(LM_INFO, tag, format, args);
    }

    public static String logMessage(String tag, String format, Object ... args) {
        return MengineLog.log(LM_MESSAGE, tag, format, args);
    }

    public static String logWarning(String tag, String format, Object ... args) {
        return MengineLog.log(LM_WARNING, tag, format, args);
    }

    public static String logFatal(String tag, String format, Object ... args) {
        return MengineLog.log(LM_FATAL, tag, format, args);
    }

    public static String logMessageRelease(String tag, String format, Object ... args) {
        return MengineLog.log(LM_MESSAGE_RELEASE, tag, format, args);
    }

    public static String logError(String tag, String format, Object ... args) {
        return MengineLog.log(LM_ERROR, tag, format, args);
    }

    public static String buildTotalMsg(String format, Object ... args) {
        StringBuilder sb = new StringBuilder();

        Formatter formatter = new Formatter(sb);

        String msg;

        try {
            msg = formatter.format(format, args).toString();
        } catch (IllegalFormatException e) {
            String error = "Catch illegal format: '" + format + "' args: '" + Arrays.toString(args) + "' exception: " + e.getLocalizedMessage();

            return error;
        }

        return msg;
    }
}
