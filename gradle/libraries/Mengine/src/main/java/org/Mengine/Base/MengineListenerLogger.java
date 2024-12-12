package org.Mengine.Base;

public interface MengineListenerLogger extends MengineServiceInterface {
    void onMengineLogger(MengineApplication application, int level, int filter, String category, String msg);
}
