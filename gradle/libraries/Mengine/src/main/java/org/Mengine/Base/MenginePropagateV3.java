package org.Mengine.Base;

@FunctionalInterface
public interface MenginePropagateV3<L, A1, A2, A3> {
    void call(L listener, MengineApplication application, A1 a1, A2 a2, A3 a3);
}
