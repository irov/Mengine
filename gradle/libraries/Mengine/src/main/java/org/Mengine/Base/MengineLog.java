package org.Mengine.Base;

import android.util.Log;

import androidx.annotation.Size;

import java.util.ArrayList;
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

    static class HistoryRecord
    {
        public int level;
        public String tag;
        public String message;
        public MengineApplication application;
    };

    private static ArrayList<HistoryRecord> m_history = new ArrayList<>();

    public static void setMengineApplication(MengineApplication application) {
        MengineLog.m_application = application;
    }

    public static void initialize(MengineActivity activity) {
        MengineLog.m_initializeBaseServices = true;

        synchronized (MengineLog.m_lock) {
            for (HistoryRecord record : MengineLog.m_history) {
                AndroidEnvironmentService_log(record.level, record.tag, record.message);

                if (record.application == null) {
                    MengineLog.m_application.onMengineLogger(record.level, record.tag, record.message);
                }
            }

            m_history.clear();
        }
    }

    public static void finalize(MengineActivity activity) {
        MengineLog.m_initializeBaseServices = false;
    }

    public static void logLevel(int level, @Size(min = 1L,max = 23L) String tag, String msg) {
        switch (level) {
            case LM_SILENT:
                break;
            case LM_FATAL:
                Log.wtf(tag, msg);
                break;
            case LM_MESSAGE_RELEASE:
                Log.w(tag, msg);
                break;
            case LM_ERROR:
                Log.e(tag, msg);
                break;
            case LM_WARNING:
                Log.w(tag, msg);
                break;
            case LM_MESSAGE:
                Log.i(tag, msg);
                break;
            case LM_INFO:
                Log.i(tag, msg);
                break;
            case LM_DEBUG:
                Log.d(tag, msg);
                break;
            case LM_VERBOSE:
                Log.v(tag, msg);
                break;
        }
    }

    private static String log(int level, @Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String totalMsg = MengineLog.buildTotalMsg(format, args);

        MengineLog.logLevel(level, tag, totalMsg);

        synchronized (MengineLog.m_lock) {
            if (MengineLog.m_initializeBaseServices == true) {
                AndroidEnvironmentService_log(level, tag, totalMsg);
            } else {
                HistoryRecord record = new HistoryRecord();
                record.level = level;
                record.tag = tag;
                record.message = totalMsg;
                record.application = MengineLog.m_application;

                MengineLog.m_history.add(record);
            }
        }

        if (MengineLog.m_application != null) {
            MengineLog.m_application.onMengineLogger(level, tag, totalMsg);
        }

        return totalMsg;
    }

    public static String logVerbose(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.log(LM_VERBOSE, tag, format, args);

        return m;
    }

    public static String logDebug(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.log(LM_DEBUG, tag, format, args);

        return m;
    }

    public static String logInfo(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        if (BuildConfig.DEBUG == false) {
            return "";
        }

        String m = MengineLog.log(LM_INFO, tag, format, args);

        return m;
    }

    public static String logMessage(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.log(LM_MESSAGE, tag, format, args);

        return m;
    }

    public static String logWarning(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.log(LM_WARNING, tag, format, args);

        return m;
    }

    public static String logFatal(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.log(LM_FATAL, tag, format, args);

        return m;
    }

    public static String logMessageRelease(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.log(LM_MESSAGE_RELEASE, tag, format, args);

        return m;
    }

    public static String logError(@Size(min = 1L,max = 23L) String tag, String format, Object ... args) {
        String m = MengineLog.log(LM_ERROR, tag, format, args);

        return m;
    }

    public static String buildTotalMsg(String format, Object ... args) {
        StringBuilder sb = new StringBuilder();

        Formatter formatter = new Formatter(sb);

        String msg;

        try {
            msg = formatter.format(format, args).toString();
        } catch (IllegalFormatException e) {
            String error = "Catch illegal format: '" + format + "' args: '" + Arrays.toString(args) + "' exception: " + e.getMessage();

            return error;
        }

        return msg;
    }
}
