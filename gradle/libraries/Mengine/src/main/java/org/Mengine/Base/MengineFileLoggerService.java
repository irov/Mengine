package org.Mengine.Base;

import android.content.Context;

import androidx.annotation.NonNull;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.Map;

public class MengineFileLoggerService extends MengineService implements MengineListenerApplication, MengineListenerActivity, MengineListenerLogger {
    public static final String SERVICE_NAME = "FileLogger";
    public static final boolean SERVICE_EMBEDDING = true;

    private static final String PREFERENCE_LOG_FILE_PATH = "mengine.file_logger.log_file_path";

    private File m_logFile;
    private File m_oldLogFile;
    private FileWriter m_writer;

    public File getLogFile() {
        return m_logFile;
    }

    public File getOldLogFile() {
        return m_oldLogFile;
    }

    private static void copyFile(File source, File dest) throws IOException {
        try (FileInputStream sourceStream = new FileInputStream(source);
             FileOutputStream destStream = new FileOutputStream(dest);
             FileChannel sourceChannel = sourceStream.getChannel();
             FileChannel destChannel = destStream.getChannel()) {
            destChannel.transferFrom(sourceChannel, 0, sourceChannel.size());
        }
    }

    private File openOldLogFile(@NonNull Context context) {
        String previousLogFilePath = MenginePreferences.getPreferenceString(PREFERENCE_LOG_FILE_PATH, null);

        if (previousLogFilePath == null) {
            return null;
        }

        File previousLogFile = new File(previousLogFilePath);

        if (previousLogFile.exists() == false ) {
            return null;
        }

        if (previousLogFile.length() == 0) {
            return null;
        }

        File oldLogFile = MengineUtils.createTempFile(context, "mng_old_log_", ".log");

        if (oldLogFile == null) {
            this.logWarning("invalid create old log file");

            return null;
        }

        try {
            this.logInfo("saved previous log file as old log: %s -> %s", previousLogFilePath, oldLogFile.getAbsolutePath());

            FileInputStream sourceStream = new FileInputStream(previousLogFile);
            FileOutputStream destStream = new FileOutputStream(oldLogFile);
            FileChannel sourceChannel = sourceStream.getChannel();
            FileChannel destChannel = destStream.getChannel();
            destChannel.transferFrom(sourceChannel, 0, sourceChannel.size());
        } catch (IOException e) {
            this.logException(e, Map.of("source", previousLogFilePath, "dest", oldLogFile.getAbsolutePath()));

            return null;
        }

        return oldLogFile;
    }

    private File openLogFile(@NonNull Context context) {
        File logFile = MengineUtils.createTempFile(context, "mng_log_", ".log");

        if (logFile == null) {
            this.logWarning("invalid create log file");

            return null;
        }

        String logFileAbsolutePath = logFile.getAbsolutePath();

        MenginePreferences.setPreferenceString(PREFERENCE_LOG_FILE_PATH, logFileAbsolutePath);

        return logFile;
    }

    @Override
    public void onAppInit(@NonNull MengineApplication application, boolean isMainProcess) throws MengineServiceInvalidInitializeException {
        if (isMainProcess == false) {
            return;
        }

        Context context = application.getApplicationContext();

        m_oldLogFile = this.openOldLogFile(context);

        m_logFile = this.openLogFile(context);

        try {
            m_writer = new FileWriter(m_logFile, true);
        } catch (IOException e) {
            this.logException(e, Map.of("file", m_logFile.getAbsolutePath()));
        }
    }

    @Override
    public void onAppFinalize(@NonNull MengineApplication application) {
        super.onAppFinalize(application);

        if (m_writer != null) {
            try {
                m_writer.flush();
                m_writer.close();
            } catch (IOException ignored) {
                // ignore
            }

            m_writer = null;
        }

        m_logFile = null;
        m_oldLogFile = null;
    }

    @Override
    public void onPause(@NonNull MengineActivity activity) {
        if (m_writer == null) {
            return;
        }

        try {
            m_writer.flush();
        } catch (IOException ignored) {
            // ignore
        }
    }

    private void writeLine(String line) {
        if (m_writer == null) {
            return;
        }

        try {
            m_writer.write(line);
            String lineSeparator = System.lineSeparator();
            m_writer.write(lineSeparator);
        } catch (IOException ignored) {
            // ignore
        }
    }

    private static String makeTimestamp() {
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS", Locale.US);
        Date date = new Date();
        String timestamp = format.format(date);
        return timestamp;
    }

    @Override
    public void onMengineLog(@NonNull MengineApplication application, @NonNull MengineParamLoggerMessage message) {
        if (message.MESSAGE_SOURCE != MengineLoggerMessageSource.MengineLoggerMessageSource_Java) {
            return;
        }

        String timestamp = MengineFileLoggerService.makeTimestamp();

        String line = String.format(Locale.US, "%s [%s] |%s| %s"
            , timestamp
            , message.MESSAGE_CATEGORY.toString()
            , message.MESSAGE_THREAD
            , message.MESSAGE_DATA);

        this.writeLine(line);
    }

    @Override
    public void onMengineException(@NonNull MengineApplication application, @NonNull MengineParamLoggerException exception) {
        String timestamp = MengineFileLoggerService.makeTimestamp();

        String line = String.format(Locale.US, "%s [EXCEPTION] %s: %s"
            , timestamp
            , exception.EXCEPTION_CATEGORY
            , exception.EXCEPTION_THROWABLE);

        this.writeLine(line);
    }
}
