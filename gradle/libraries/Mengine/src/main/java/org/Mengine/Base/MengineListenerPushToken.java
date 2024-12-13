package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerPushToken extends MengineServiceInterface {
    void onMenginePushToken(@NonNull MengineApplication application, String token);
}
