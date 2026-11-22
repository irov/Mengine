package org.Mengine.Base;

import androidx.annotation.AnyThread;
import androidx.annotation.MainThread;
import androidx.annotation.NonNull;

import java.util.concurrent.atomic.AtomicBoolean;

public abstract class MengineOperation<T> {
    private static final MengineTag TAG = MengineTag.of("MNGOperation");

    private final String m_name;
    private final AtomicBoolean m_completed;

    protected MengineOperation(@NonNull String name) {
        m_name = name;
        m_completed = new AtomicBoolean(false);
    }

    @AnyThread
    public boolean isCompleted() {
        return m_completed.get();
    }

    @AnyThread
    public final void complete(T result) {
        if (m_completed.compareAndSet(false, true) == false) {
            MengineLog.logError(MengineOperation.TAG, "operation '%s' already completed"
                , m_name
            );

            return;
        }

        MengineUtils.performOnMainThread(() -> {
            this.onComplete(result);
        });
    }

    @MainThread
    protected abstract void onComplete(T result);
}
