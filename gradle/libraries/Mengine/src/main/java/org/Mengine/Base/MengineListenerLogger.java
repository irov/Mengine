package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerLogger extends MengineServiceInterface {
    void onMengineLog(@NonNull MengineApplication application, @NonNull MengineParamLoggerMessage message);
    void onMengineException(@NonNull MengineApplication application, @NonNull MengineParamLoggerException exception);
}
