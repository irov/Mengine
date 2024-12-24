package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineSemaphoreListener {
    private Object m_impl;

    MengineSemaphoreListener(@NonNull Object impl) {
        m_impl = impl;
    }

    public void destroy() {
        MengineNative.AnroidEnvironmentService_destroyMengineSemaphoreListener(m_impl);

        m_impl = null;
    }

    public void call() {
        MengineNative.AnroidEnvironmentService_callMengineSemaphoreListener(m_impl);
    }
}