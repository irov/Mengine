package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineFragmentAcquisition extends MengineFragment<MengineListenerAcquisition> {
    public static MengineFragmentAcquisition INSTANCE = null;

    MengineFragmentAcquisition() {
        super(MengineListenerAcquisition.class);

        INSTANCE = this;
    }

    public void changeADID(@NonNull String adid) {
        this.propagate(MengineListenerAcquisition::onADIDChange, adid);
    }

    public void changeAcquisition(@NonNull MengineParamAcquisition acquisition) {
        this.propagate(MengineListenerAcquisition::onAcquisitionChange, acquisition);
    }
}