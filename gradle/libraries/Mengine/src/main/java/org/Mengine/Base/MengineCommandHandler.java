package org.Mengine.Base;

import android.os.Handler;
import android.os.Message;

public class MengineCommandHandler extends Handler {
    public static final String TAG = "MengineCommandHandler";

    MengineActivity m_activity;

    public MengineCommandHandler(MengineActivity activity) {
        m_activity = activity;
    }

    public void handleMessage(Message msg) {
        // ToDo
    }
}