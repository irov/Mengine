package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MenginePluginExtensionInterface {
    void onExtensionInitialize(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException;
    void onExtensionFinalize(@NonNull MengineApplication application);
}
