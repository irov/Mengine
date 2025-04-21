package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineFragmentInterface {
    default void explainServiceListeners(@NonNull MengineServiceInterface service) {
        //Override
    }
}