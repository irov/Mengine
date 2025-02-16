package org.Mengine.Base;

import android.util.Log;

import androidx.annotation.Size;

import java.util.Arrays;
import java.util.HashSet;
import java.util.IllegalFormatException;
import java.util.Set;

public class MengineLog {
    public final static int LM_SILENT = 0;
    public final static int LM_FATAL = 1;
    public final static int LM_MESSAGE_RELEASE = 2;
    public final static int LM_ERROR = 3;
    public final static int LM_WARNING = 4;
    public final static int LM_MESSAGE = 5;
    public final static int LM_INFO = 6;
    public final static int LM_DEBUG = 7;
    public final static int LM_VERBOSE = 8;

    public final static int LFILTER_NONE = 0 << 0;
    public final static int LFILTER_PROTECTED = 1 << 1;
    public final static int LFILTER_HTTP = 1 << 2;
    public final static int LFILTER_EXCEPTION = 1 << 3;

    private static final Set<String> m_singles = new HashSet<>();

    static class HistoryRecord {
        public int level;
        public int filter;
        public String tag;
        public String message;
        public MengineApplication application;
    }

    public static boolean isFilter(int filter, int flag) {
        return (filter & flag) == flag;
    }

    public static void logNativeLevel(int level, @Size(min = 1L,max = 23L) String tag, String msg) {
        switch (level) {
            case LM_SILENT:
                break;
            case LM_FATAL:
                Log.wtf(tag, "\uD83D\uDFE5 " + msg);
                break;
            case LM_MESSAGE_RELEASE:
                Log.w(tag, msg);
                break;
            case LM_ERROR:
                Log.e(tag, "\uD83D\uDD34 " + msg);
                break;
            case LM_WARNING:
                Log.w(tag, "\uD83D\uDFE1 " + msg);
                break;
            case LM_MESSAGE:
                Log.i(tag, "\uD83D\uDCDD " + msg);
                break;
            case LM_INFO:
                Log.i(tag, "\uD83D\uDFE2 " + msg);
                break;
            case LM_DEBUG:
                Log.d(tag, "\uD83D\uDD0D " + msg);
                break;
            case LM_VERBOSE:
                Log.v(tag, msg);
                break;
        }
    }

    private static String log(int level, @Size(min = 1L,max = 23L) String tag, int filter, String format, Object ... args) {
        String message = MengineLog.buildTotalMsg(format, args);

        MengineLog.logString(level, tag, filter, message);

        return message;
    }

    private static void logString(int level, @Size(min = 1L,max = 23L) String tag, int filter, String message) {
        MengineLog.logNativeLevel(level, tag, message);

        if (level >= LM_INFO) {
            return;
        }

        MengineApplication application = MengineApplication.getSharedInstance();

        application.onMengineLogger(level, filter, tag, message);
    }

    public static String logVerbose(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.log(LM_VERBOSE, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logDebug(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.log(LM_DEBUG, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logInfo(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.log(LM_INFO, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logMessage(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.log(LM_MESSAGE, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logWarning(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.log(LM_WARNING, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logError(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.log(LM_ERROR, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logErrorException(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.log(LM_ERROR, tag, MengineLog.LFILTER_EXCEPTION, format, args);

        return m;
    }

    public static String logMessageRelease(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.log(LM_MESSAGE_RELEASE, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logFatal(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.log(LM_FATAL, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logSingleMessage(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.buildTotalMsg(format, args);

        if (MengineLog.m_singles.contains(m) == true) {
            return m;
        }

        MengineLog.logString(LM_MESSAGE, tag, MengineLog.LFILTER_NONE, m);

        return m;
    }

    public static String logSingleWarning(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.buildTotalMsg(format, args);

        if (MengineLog.m_singles.contains(m) == true) {
            return m;
        }

        MengineLog.logString(LM_WARNING, tag, MengineLog.LFILTER_NONE, m);

        return m;
    }

    public static String logSingleError(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.buildTotalMsg(format, args);

        if (MengineLog.m_singles.contains(m) == true) {
            return m;
        }

        MengineLog.logString(LM_ERROR, tag, MengineLog.LFILTER_NONE, m);

        return m;
    }

    public static String buildTotalMsg(String format, Object ... args) {
        if (args == null || args.length == 0) {
            return format;
        }

        try {
            String msg = String.format(format, args);

            return msg;
        } catch (final IllegalFormatException e) {
            String error = "Catch illegal format: '" + format + "' args: '" + Arrays.toString(args) + "' exception: " + e.getMessage();

            return error;
        }
    }
}
