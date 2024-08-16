package org.Mengine.Base;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

public class MengineCommandHandler extends Handler {
    public static final String TAG = "MengineCommandHandler";

    MengineActivity m_activity;

    public MengineCommandHandler(Looper looper, MengineActivity activity) {
        super(looper);

        m_activity = activity;
    }

    public void handleMessage(Message msg) {
        // ToDo
    }
}