package org.Mengine.Base;

import android.os.Bundle;

public interface MenginePluginProxyActivityListener extends MenginePluginInterface {
    void onProxyActivityCreate(MenginePluginProxyActivity activity, Bundle savedInstanceState, Runnable complete) throws MenginePluginInvalidInitializeException;
}