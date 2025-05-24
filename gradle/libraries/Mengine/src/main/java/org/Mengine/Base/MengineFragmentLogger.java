package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineFragmentLogger extends MengineFragment<MengineListenerLogger> {
    public static MengineFragmentLogger INSTANCE = null;

    MengineFragmentLogger() {
        super(MengineListenerLogger.class);

        INSTANCE = this;
    }

    public void log(@NonNull MengineParamLoggerMessage message) {
        this.propagate(MengineListenerLogger::onMengineLog, message);
    }

    public void exception(@NonNull MengineParamLoggerException exception) {
        this.propagate(MengineListenerLogger::onMengineException, exception);
    }
}
