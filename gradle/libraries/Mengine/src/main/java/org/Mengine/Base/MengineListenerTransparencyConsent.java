package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerTransparencyConsent extends MengineServiceInterface {
    void onMengineTransparencyConsent(@NonNull MengineApplication application, MengineTransparencyConsentParam tcParam);
}
