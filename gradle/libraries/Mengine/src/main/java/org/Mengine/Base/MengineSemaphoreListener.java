package org.Mengine.Base;

public class MengineSemaphoreListener {
    private static native void AnroidEnvironmentService_callMengineSemaphoreListener(Object impl);
    private static native void AnroidEnvironmentService_destroyMengineSemaphoreListener(Object impl);

    private Object m_impl;

    MengineSemaphoreListener(Object impl) {
        m_impl = impl;
    }

    public void destroy() {
        AnroidEnvironmentService_destroyMengineSemaphoreListener(m_impl);
        m_impl = null;
    }

    public void call() {
        AnroidEnvironmentService_callMengineSemaphoreListener(m_impl);
    }
}