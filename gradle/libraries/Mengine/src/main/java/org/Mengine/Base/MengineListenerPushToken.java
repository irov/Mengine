package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerPushToken extends MengineServiceInterface {
    void onMengineChangePushToken(@NonNull MengineApplication application, String token);
}
