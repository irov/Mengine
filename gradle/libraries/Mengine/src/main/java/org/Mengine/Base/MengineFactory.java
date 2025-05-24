package org.Mengine.Base;

import androidx.annotation.NonNull;

@FunctionalInterface
public interface MengineFactory {
    MengineFactorable create(@NonNull Object ... args);
}

