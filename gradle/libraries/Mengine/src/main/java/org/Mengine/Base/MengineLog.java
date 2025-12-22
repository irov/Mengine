package org.Mengine.Base;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import java.util.Arrays;
import java.util.HashSet;
import java.util.IllegalFormatException;
import java.util.Map;
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
    private static final Object m_singlesLock = new Object();

    public static boolean isFilter(int filter, int flag) {
        return (filter & flag) == flag;
    }

    public static void logNativeLevel(int level, @NonNull MengineTag category, @NonNull String msg) {
        switch (level) {
            case LM_SILENT:
                break;
            case LM_FATAL:
                Log.wtf(category.toString(), "\uD83D\uDFE5 " + msg);
                break;
            case LM_MESSAGE_RELEASE:
                Log.w(category.toString(), "\uD83D\uDCDD " + msg);
                break;
            case LM_ERROR:
                Log.e(category.toString(), "\uD83D\uDD34 " + msg);
                break;
            case LM_WARNING:
                Log.w(category.toString(), "\uD83D\uDFE1 " + msg);
                break;
            case LM_MESSAGE:
                Log.i(category.toString(), "\uD83D\uDCDD " + msg);
                break;
            case LM_INFO:
                Log.i(category.toString(), "\uD83D\uDFE2 " + msg);
                break;
            case LM_DEBUG:
                Log.d(category.toString(), "\uD83D\uDD0D " + msg);
                break;
            case LM_VERBOSE:
                Log.v(category.toString(), msg);
                break;
        }
    }

    private static String log(int level, @NonNull MengineTag category, int filter, @NonNull String format, Object ... args) {
        String message = MengineLog.buildTotalMsg(format, args);

        MengineLog.logString(level, category, filter, message);

        return message;
    }

    private static void logString(int level, @NonNull MengineTag category, int filter, String data) {
        MengineLog.logNativeLevel(level, category, data);

        String thread = MengineUtils.getCurrentThreadName();

        MengineUtils.Code code = MengineUtils.getCurrentThreadCode(6);

        MengineParamLoggerMessage message = new MengineParamLoggerMessage(MengineLoggerMessageSource.MengineLoggerMessageSource_Java, category, thread, level, filter, code.file, code.line, code.method, data);

        MengineFragmentLogger.INSTANCE.log(message);
    }

    public static String logVerbose(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.log(LM_VERBOSE, category, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logDebug(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.log(LM_DEBUG, category, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logInfo(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        String m = MengineLog.log(LM_INFO, category, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logMessage(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        String m = MengineLog.log(LM_MESSAGE, category, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logMessageRelease(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        String m = MengineLog.log(LM_MESSAGE_RELEASE, category, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logWarning(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        String m = MengineLog.log(LM_WARNING, category, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logError(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        String m = MengineLog.log(LM_ERROR, category, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logErrorException(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        String m = MengineLog.log(LM_ERROR, category, MengineLog.LFILTER_EXCEPTION, format, args);

        return m;
    }

    public static void logException(@NonNull MengineTag category, @NonNull Throwable e, @NonNull Map<String, Object> attributes) {
        String message = e.toString();
        String trace = Log.getStackTraceString(e);
        MengineLog.logError(category, "%s\n%s", message, trace);

        MengineParamLoggerException exception = new MengineParamLoggerException(category, e, attributes);

        MengineFragmentLogger.INSTANCE.exception(exception);
    }

    public static String logFatal(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        String m = MengineLog.log(LM_FATAL, category, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    public static String logSingleMessage(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.buildTotalMsg(format, args);

        synchronized (MengineLog.m_singlesLock) {
            if (MengineLog.m_singles.contains(m) == true) {
                return m;
            }

            MengineLog.m_singles.add(m);
        }

        MengineLog.logString(LM_MESSAGE, category, MengineLog.LFILTER_NONE, m);

        return m;
    }

    public static String logSingleWarning(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        String m = MengineLog.buildTotalMsg(format, args);

        synchronized (MengineLog.m_singlesLock) {
            if (MengineLog.m_singles.contains(m) == true) {
                return m;
            }

            MengineLog.m_singles.add(m);
        }

        MengineLog.logString(LM_WARNING, category, MengineLog.LFILTER_NONE, m);

        return m;
    }

    public static String logSingleError(@NonNull MengineTag category, @NonNull String format, Object ... args) {
        String m = MengineLog.buildTotalMsg(format, args);

        synchronized (MengineLog.m_singlesLock) {
            if (MengineLog.m_singles.contains(m) == true) {
                return m;
            }

            MengineLog.m_singles.add(m);
        }

        MengineLog.logString(LM_ERROR, category, MengineLog.LFILTER_NONE, m);

        return m;
    }

    public static String buildTotalMsg(@NonNull String format, Object ... args) {
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
