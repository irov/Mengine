package org.Mengine.Base;

public interface MenginePluginLoggerListener {
    public void onMengineLogger(MengineActivity activity, String category, int level, int filter, int color, String msg);
}
