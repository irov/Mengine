package org.Mengine.Base;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import androidx.annotation.NonNull;

public class MengineCommandHandler extends Handler {
    public static final MengineTag TAG = MengineTag.of("MengineCommandHandler");

    protected final MengineActivity m_activity;

    public MengineCommandHandler(@NonNull Looper looper, @NonNull MengineActivity activity) {
        super(looper);

        m_activity = activity;
    }

    @Override
    public void handleMessage(@NonNull Message msg) {
        // ToDo
    }
}