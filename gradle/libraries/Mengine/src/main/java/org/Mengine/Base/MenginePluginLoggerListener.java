package org.Mengine.Base;

public interface MenginePluginLoggerListener {
    void onMengineLogger(MengineApplication application, String category, int level, int filter, int color, String msg);
}
