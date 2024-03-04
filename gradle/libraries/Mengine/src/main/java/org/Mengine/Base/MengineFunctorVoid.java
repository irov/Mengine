package org.Mengine.Base;

public class MengineFunctorVoid {
    private static native void AnroidEnvironmentService_callMengineFunctorVoid(Object impl);
    private static native void AnroidEnvironmentService_destroyMengineFunctorVoid(Object impl);

    private Object m_impl;

    MengineFunctorVoid(Object impl) {
        m_impl = impl;
    }

    public void destroy() {
        AnroidEnvironmentService_destroyMengineFunctorVoid(m_impl);
        m_impl = null;
    }

    public void call() {
        AnroidEnvironmentService_callMengineFunctorVoid(m_impl);
    };
}