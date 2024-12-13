package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerLogger extends MengineServiceInterface {
    void onMengineLogger(@NonNull MengineApplication application, int level, int filter, String category, String msg);
}
