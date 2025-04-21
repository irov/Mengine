package org.Mengine.Base;

@FunctionalInterface
public interface MenginePropagateV1<L, A1> {
    void call(L listener, MengineApplication application, A1 a1);
}
