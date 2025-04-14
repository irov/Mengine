package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineFragmentTransparencyConsent extends MengineFragment<MengineListenerTransparencyConsent> {
    public static MengineFragmentTransparencyConsent INSTANCE = null;

    MengineFragmentTransparencyConsent() {
        super(MengineListenerTransparencyConsent.class);

        INSTANCE = this;
    }

    public void transparencyConsent(@NonNull MengineTransparencyConsentParam tcParam) {
        this.propagate(MengineListenerTransparencyConsent::onMengineTransparencyConsent, tcParam);
    }
}