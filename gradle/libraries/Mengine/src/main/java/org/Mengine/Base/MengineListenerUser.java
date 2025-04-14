package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerUser extends MengineServiceInterface {
    void onMengineChangeUserId(@NonNull MengineApplication application, String userId);
    void onMengineRemoveUserData(@NonNull MengineApplication application);
}