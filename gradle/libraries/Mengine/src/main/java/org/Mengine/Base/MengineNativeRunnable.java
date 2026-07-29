package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.nio.ByteBuffer;

public final class MengineNativeRunnable implements Runnable, AutoCloseable {
    private ByteBuffer m_nativeRunnable;

    public MengineNativeRunnable(@NonNull ByteBuffer nativeRunnable) {
        m_nativeRunnable = nativeRunnable;
    }

    @Nullable
    private synchronized ByteBuffer takeNativeRunnable() {
        ByteBuffer nativeRunnable = m_nativeRunnable;
        m_nativeRunnable = null;

        return nativeRunnable;
    }

    @Override
    public void run() {
        ByteBuffer nativeRunnable = this.takeNativeRunnable();

        if (nativeRunnable == null) {
            return;
        }

        MengineNative.AndroidPlatform_invokeNativeRunnable(nativeRunnable);
    }

    @Override
    public void close() {
        ByteBuffer nativeRunnable = this.takeNativeRunnable();

        if (nativeRunnable == null) {
            return;
        }

        MengineNative.AndroidPlatform_releaseNativeRunnable(nativeRunnable);
    }
}
