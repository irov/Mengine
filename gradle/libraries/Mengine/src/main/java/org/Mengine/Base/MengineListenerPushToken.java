package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerPushToken extends MengineServiceInterface {
    void onMengineSetPushToken(@NonNull MengineApplication application, String token);
}
