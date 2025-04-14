package org.Mengine.Base;

@FunctionalInterface
public interface MenginePropagateV2<L, A1, A2> {
    void call(L listener, MengineApplication application, A1 a1, A2 a2);
}
