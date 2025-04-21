package org.Mengine.Base;

@FunctionalInterface
public interface MenginePropagateB4<L, A1, A2, A3, A4> {
    boolean call(L listener, MengineApplication application, A1 a1, A2 a2, A3 a3, A4 a4);
}
