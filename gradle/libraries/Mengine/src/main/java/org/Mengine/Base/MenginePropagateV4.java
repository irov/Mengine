package org.Mengine.Base;

@FunctionalInterface
public interface MenginePropagateV4<L, A1, A2, A3, A4> {
    void call(L listener, MengineApplication application, A1 a1, A2 a2, A3 a3, A4 a4);
}
