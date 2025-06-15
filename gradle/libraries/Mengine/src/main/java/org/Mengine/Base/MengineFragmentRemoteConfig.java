package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class MengineFragmentRemoteConfig extends MengineFragment<MengineListenerRemoteConfig> {
    public static MengineFragmentRemoteConfig INSTANCE = null;

    private Map<String, JSONObject> m_configs = new HashMap<>();

    MengineFragmentRemoteConfig() {
        super(MengineListenerRemoteConfig.class);

        INSTANCE = this;
    }

    public Map<String, JSONObject> getRemoteConfigs() {
        synchronized (this) {
            return m_configs;
        }
    }

    public JSONObject getRemoteConfigValue(String key) {
        synchronized (this) {
            JSONObject value = m_configs.get(key);

            return value;
        }
    }

    public void remoteConfigFetch(@NonNull Map<String, JSONObject> configs) {
        synchronized (this) {
            m_configs = configs;
        }

        this.propagate(MengineListenerRemoteConfig::onMengineRemoteConfigFetch, configs);
    }
}