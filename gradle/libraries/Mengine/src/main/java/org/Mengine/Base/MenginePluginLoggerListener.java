package org.Mengine.Base;

public interface MenginePluginLoggerListener extends MenginePluginInterface {
    void onMengineLogger(MengineApplication application, int level, int filter, String category, String msg);
}
