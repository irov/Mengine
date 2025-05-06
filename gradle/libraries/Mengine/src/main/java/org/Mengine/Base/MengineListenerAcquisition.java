package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerAcquisition extends MengineServiceInterface {
    void onADIDChange(@NonNull MengineApplication application, @NonNull String adid);
    void onAcquisitionChange(@NonNull MengineApplication application, @NonNull MengineParamAcquisition acquisition);
}