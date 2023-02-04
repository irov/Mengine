package org.Mengine.Base;

public interface MengineLoggerListener {
    public void onMengineLogger(MengineActivity activity, String category, int level, int filter, int color, String msg);
}
