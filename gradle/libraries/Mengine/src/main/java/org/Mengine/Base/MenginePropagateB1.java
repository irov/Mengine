package org.Mengine.Base;

@FunctionalInterface
public interface MenginePropagateB1<L, A1> {
    boolean call(L listener, MengineApplication application, A1 a1);
}
