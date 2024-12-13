package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerSessionId extends MengineServiceInterface {
    void onMengineSetSessionId(@NonNull MengineApplication application, String sessionId);
    void onMengineRemoveSessionData(@NonNull MengineApplication application);
}