package org.Mengine.Base;

public interface MenginePluginLoggerListener extends MenginePluginInterface {
    void onMengineLogger(MengineApplication application, int level, String category, String msg);
}
