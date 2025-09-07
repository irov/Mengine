package org.Mengine.Base;

import android.content.Context;

import androidx.annotation.NonNull;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.Map;

public class MengineFileLoggerService extends MengineService implements MengineListenerApplication, MengineListenerLogger {
    public static final String SERVICE_NAME = "FileLogger";
    public static final boolean SERVICE_EMBEDDING = true;

    private FileWriter m_writer;

    @Override
    public void onAppInit(@NonNull MengineApplication application, boolean isMainProcess) throws MengineServiceInvalidInitializeException {
        if (isMainProcess == false) {
            return;
        }

        Context context = application.getApplicationContext();

        File logFile = MengineUtils.createTempFile(context, "mng_log_", ".log");

        try {
            m_writer = new FileWriter(logFile, true);
        } catch (IOException e) {
            this.logException(e, Map.of("file", logFile.getAbsolutePath()));
        }
    }

    @Override
    public void onAppFinalize(@NonNull MengineApplication application) {
        super.onAppFinalize(application);

        if (m_writer != null) {
            try {
                m_writer.close();
            } catch (IOException ignored) {
                // ignore
            }

            m_writer = null;
        }
    }

    private void writeLine(String line) {
        if (m_writer == null) {
            return;
        }

        try {
            m_writer.write(line);
            m_writer.flush();
        } catch (IOException ignored) {
            // ignore
        }
    }

    private static String makeTimestamp() {
        return new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS", Locale.US).format(new Date());
    }

    @Override
    public void onMengineLog(@NonNull MengineApplication application, @NonNull MengineParamLoggerMessage message) {
        String timestamp = MengineFileLoggerService.makeTimestamp();

        String line = String.format(Locale.US, "%s |%s| [%s] %s"
            , timestamp
            , message.MESSAGE_THREAD
            , message.MESSAGE_CATEGORY.toString()
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
