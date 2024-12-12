package org.Mengine.Base;

public interface MengineListenerSessionId extends MengineServiceInterface {
    void onMengineSetSessionId(MengineApplication application, String sessionId);
    void onMengineRemoveSessionData(MengineApplication application);
}