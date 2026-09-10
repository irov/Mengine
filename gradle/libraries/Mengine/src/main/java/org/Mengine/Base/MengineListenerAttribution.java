package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.Size;

public interface MengineListenerAttribution extends MengineServiceInterface {
    void onMengineAttribution(@NonNull MengineApplication application, @NonNull @Size(min = 1, max = 22) String name, @Nullable Object value);
}
