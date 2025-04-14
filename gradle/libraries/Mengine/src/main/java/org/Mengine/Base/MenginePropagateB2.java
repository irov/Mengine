package org.Mengine.Base;

@FunctionalInterface
public interface MenginePropagateB2<L, A1, A2> {
    boolean call(L listener, MengineApplication application, A1 a1, A2 a2);
}
