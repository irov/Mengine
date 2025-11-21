package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class MengineFragmentRemoteConfig extends MengineFragment<MengineListenerRemoteConfig> {
    public static MengineFragmentRemoteConfig INSTANCE = null;

    private String m_installationId;
    private Map<String, JSONObject> m_configs = new HashMap<>();
    private Map<String, Integer> m_ids = new HashMap<>();
    private final Object m_syncronizationConfigs = new Object();

    MengineFragmentRemoteConfig() {
        super(MengineListenerRemoteConfig.class);

        INSTANCE = this;
    }

    public void setInstallationId(@NonNull String installationId) {
        synchronized (m_syncronizationConfigs) {
            m_installationId = installationId;
        }
    }

    public String getInstallationId() {
        synchronized (m_syncronizationConfigs) {
            return m_installationId;
        }
    }

    public Map<String, JSONObject> getRemoteConfigs() {
        synchronized (m_syncronizationConfigs) {
            return m_configs;
        }
    }

    public Map<String, Integer> getRemoteConfigIds() {
        synchronized (m_syncronizationConfigs) {
            return m_ids;
        }
    }

    public JSONObject getRemoteConfigValue(String key) {
        synchronized (m_syncronizationConfigs) {
            JSONObject value = m_configs.get(key);

            return value;
        }
    }

    public void remoteConfigFetch(@NonNull Map<String, JSONObject> configs, @NonNull Map<String, Integer> ids) {
        synchronized (m_syncronizationConfigs) {
            m_configs = configs;
            m_ids = ids;
        }
    }

    public void remoteConfigPropagate(boolean updated) {
        Map<String, JSONObject> configs = this.getRemoteConfigs();
        Map<String, Integer> ids = this.getRemoteConfigIds();

        this.propagate(MengineListenerRemoteConfig::onMengineRemoteConfigFetch, updated, configs, ids);
    }
}