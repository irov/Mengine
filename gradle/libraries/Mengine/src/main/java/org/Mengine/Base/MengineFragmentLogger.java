package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineFragmentLogger extends MengineFragment<MengineListenerLogger> {
    public static MengineFragmentLogger INSTANCE = null;

    MengineFragmentLogger() {
        super(MengineListenerLogger.class);

        INSTANCE = this;
    }

    public void log(@NonNull MengineLoggerMessageParam message) {
        this.propagate(MengineListenerLogger::onMengineLog, message);
    }
}
