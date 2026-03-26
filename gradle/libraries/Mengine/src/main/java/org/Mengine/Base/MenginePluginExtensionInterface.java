package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MenginePluginExtensionInterface {
    void onExtensionInitialize(@NonNull MengineApplication application);
    void onExtensionFinalize(@NonNull MengineApplication application);
}
