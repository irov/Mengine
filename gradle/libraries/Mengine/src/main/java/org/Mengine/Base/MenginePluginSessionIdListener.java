package org.Mengine.Base;

public interface MenginePluginSessionIdListener extends MenginePluginInterface {
    void onMengineSetSessionId(MengineApplication application, String sessionId);
    void onMengineRemoveSessionData(MengineApplication application);
}