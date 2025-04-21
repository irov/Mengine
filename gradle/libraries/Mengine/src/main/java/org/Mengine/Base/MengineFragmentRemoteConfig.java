package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

import java.util.Map;

public class MengineFragmentRemoteConfig extends MengineFragment<MengineListenerRemoteConfig> {
    public static MengineFragmentRemoteConfig INSTANCE = null;

    MengineFragmentRemoteConfig() {
        super(MengineListenerRemoteConfig.class);

        INSTANCE = this;
    }

    public void remoteConfigFetch(@NonNull Map<String, JSONObject> configs) {
        this.propagate(MengineListenerRemoteConfig::onMengineRemoteConfigFetch, configs);
    }
}