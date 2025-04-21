package org.Mengine.Base;

@FunctionalInterface
public interface MenginePropagateV0<L> {
    void call(L listener, MengineApplication application);
}

