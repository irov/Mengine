package org.Mengine.Base;

public interface MenginePluginLoggerListener extends MenginePluginInterface {
    void onMengineLogger(MengineApplication application, String category, int level, int filter, int color, String msg);
}
