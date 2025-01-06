package org.Mengine.Base;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import com.google.errorprone.annotations.FormatMethod;
import com.google.errorprone.annotations.FormatString;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Formatter;
import java.util.IllegalFormatException;

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
    public final static int LFILTER_ANDROID = 1 << 2;
    public final static int LFILTER_HTTP = 1 << 3;
    public final static int LFILTER_IMMEDIATE = 1 << 4;
    public final static int LFILTER_EXCEPTION = 1 << 5;

    private static MengineApplication m_application;
    private static boolean m_initializeBaseServices = false;
    private static final Object m_lock = new Object();

    static class HistoryRecord
    {
        public int level;
        public int filter;
        public String tag;
        public String message;
        public MengineApplication application;
    }

    private static final ArrayList<HistoryRecord> m_history = new ArrayList<>();

    public static void setMengineApplication(@NonNull MengineApplication application) {
        MengineLog.m_application = application;
    }

    public static void removeMengineApplication() {
        MengineLog.m_application = null;
    }

    public static void initializeBaseServices() {
        MengineLog.m_initializeBaseServices = true;

        synchronized (MengineLog.m_lock) {
            for (HistoryRecord record : MengineLog.m_history) {
                MengineNative.AndroidEnvironmentService_log(record.level, record.tag, record.message);

                if (record.application == null) {
                    MengineLog.m_application.onMengineLogger(record.level, record.filter, record.tag, record.message);
                }
            }

            m_history.clear();
        }
    }

    public static void finalizeBaseServices() {
        MengineLog.m_initializeBaseServices = false;
    }

    public static boolean isFilter(int filter, int flag) {
        return (filter & flag) == flag;
    }

    public static void logLevel(int level, @Size(min = 1L,max = 23L) String tag, String msg) {
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

    @FormatMethod
    private static String log(int level, @Size(min = 1L,max = 23L) String tag, int filter, @FormatString String format, Object ... args) {
        String message = MengineLog.buildTotalMsg(format, args);

        MengineLog.logLevel(level, tag, message);

        if (level >= LM_INFO) {
            return message;
        }

        synchronized (MengineLog.m_lock) {
            if (MengineLog.m_initializeBaseServices == true) {
                MengineNative.AndroidEnvironmentService_log(level, tag, message);
            } else {
                HistoryRecord record = new HistoryRecord();
                record.level = level;
                record.filter = filter;
                record.tag = tag;
                record.message = message;
                record.application = MengineLog.m_application;

                MengineLog.m_history.add(record);
            }
        }

        if (MengineLog.m_application != null) {
            MengineLog.m_application.onMengineLogger(level, MengineLog.LFILTER_NONE, tag, message);
        }

        return message;
    }

    @FormatMethod
    public static String logVerbose(@Size(min = 1L,max = 23L) String tag, @FormatString String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.log(LM_VERBOSE, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    @FormatMethod
    public static String logDebug(@Size(min = 1L,max = 23L) String tag, @FormatString String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.log(LM_DEBUG, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    @FormatMethod
    public static String logInfo(@Size(min = 1L,max = 23L) String tag, @FormatString String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.log(LM_INFO, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    @FormatMethod
    public static String logMessage(@Size(min = 1L,max = 23L) String tag, @FormatString String format, Object ... args) {
        String m = MengineLog.log(LM_MESSAGE, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    @FormatMethod
    public static String logWarning(@Size(min = 1L,max = 23L) String tag, @FormatString String format, Object ... args) {
        String m = MengineLog.log(LM_WARNING, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    @FormatMethod
    public static String logError(@Size(min = 1L,max = 23L) String tag, @FormatString String format, Object ... args) {
        String m = MengineLog.log(LM_ERROR, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    @FormatMethod
    public static String logErrorException(@Size(min = 1L,max = 23L) String tag, @FormatString String format, Object ... args) {
        String m = MengineLog.log(LM_ERROR, tag, MengineLog.LFILTER_EXCEPTION, format, args);

        return m;
    }

    @FormatMethod
    public static String logMessageRelease(@Size(min = 1L,max = 23L) String tag, @FormatString String format, Object ... args) {
        String m = MengineLog.log(LM_MESSAGE_RELEASE, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    @FormatMethod
    public static String logFatal(@Size(min = 1L,max = 23L) String tag, @FormatString String format, Object ... args) {
        String m = MengineLog.log(LM_FATAL, tag, MengineLog.LFILTER_NONE, format, args);

        return m;
    }

    @FormatMethod
    public static String buildTotalMsg(@FormatString String format, Object ... args) {
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
