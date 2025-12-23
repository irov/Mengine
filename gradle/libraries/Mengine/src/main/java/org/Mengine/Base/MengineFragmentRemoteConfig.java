package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONArray;
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

    public boolean hasRemoteConfig(@NonNull String name) {
        synchronized (m_syncronizationConfigs) {
            return m_configs.containsKey(name);
        }
    }

    public JSONObject getRemoteConfig(@NonNull String name) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            return config;
        }
    }

    public boolean hasRemoteConfigKey(@NonNull String name, @NonNull String key) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return false;
            }

            return config.has(key);
        }
    }

    public boolean getRemoteConfigValueBoolean(@NonNull String name, @NonNull String key, boolean defaultValue) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return defaultValue;
            }

            return config.optBoolean(key, defaultValue);
        }
    }

    public int getRemoteConfigValueInteger(@NonNull String name, @NonNull String key, int defaultValue) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return defaultValue;
            }

            return config.optInt(key, defaultValue);
        }
    }

    public long getRemoteConfigValueLong(@NonNull String name, @NonNull String key, long defaultValue) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return defaultValue;
            }

            return config.optLong(key, defaultValue);
        }
    }

    public double getRemoteConfigValueDouble(@NonNull String name, @NonNull String key, double defaultValue) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return defaultValue;
            }

            return config.optDouble(key, defaultValue);
        }
    }

    public String getRemoteConfigValueString(@NonNull String name, @NonNull String key, String defaultValue) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return defaultValue;
            }

            return config.optString(key, defaultValue);
        }
    }

    public JSONObject getRemoteConfigValueJSONObject(@NonNull String name, @NonNull String key) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return null;
            }

            return config.optJSONObject(key);
        }
    }

    public JSONArray getRemoteConfigValueJSONArray(@NonNull String name, @NonNull String key) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return null;
            }

            return config.optJSONArray(key);
        }
    }

    public boolean hasRemoteConfigSectionKey(@NonNull String name, @NonNull String section, @NonNull String key) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return false;
            }

            JSONObject value = config.optJSONObject(section);

            if (value == null) {
                return false;
            }

            return value.has(key);
        }
    }

    public boolean getRemoteConfigSectionValueBoolean(@NonNull String name, @NonNull String section, @NonNull String key, boolean defaultValue) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return defaultValue;
            }

            JSONObject value = config.optJSONObject(section);

            if (value == null) {
                return defaultValue;
            }

            return value.optBoolean(key, defaultValue);
        }
    }

    public int getRemoteConfigSectionValueInteger(@NonNull String name, @NonNull String section, @NonNull String key, int defaultValue) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return defaultValue;
            }

            JSONObject value = config.optJSONObject(section);

            if (value == null) {
                return defaultValue;
            }

            return value.optInt(key, defaultValue);
        }
    }

    public long getRemoteConfigSectionValueLong(@NonNull String name, @NonNull String section, @NonNull String key, long defaultValue) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return defaultValue;
            }

            JSONObject value = config.optJSONObject(section);

            if (value == null) {
                return defaultValue;
            }

            return value.optLong(key, defaultValue);
        }
    }

    public double getRemoteConfigSectionValueDouble(@NonNull String name, @NonNull String section, @NonNull String key, double defaultValue) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return defaultValue;
            }

            JSONObject value = config.optJSONObject(section);

            if (value == null) {
                return defaultValue;
            }

            return value.optDouble(key, defaultValue);
        }
    }

    public String getRemoteConfigSectionValueString(@NonNull String name, @NonNull String section, @NonNull String key, String defaultValue) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return defaultValue;
            }

            JSONObject value = config.optJSONObject(section);

            if (value == null) {
                return defaultValue;
            }

            return value.optString(key, defaultValue);
        }
    }

    public JSONObject getRemoteConfigSectionValueJSONObject(@NonNull String name, @NonNull String section, @NonNull String key) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return null;
            }

            JSONObject value = config.optJSONObject(section);

            if (value == null) {
                return null;
            }

            return value.optJSONObject(key);
        }
    }

    public JSONArray getRemoteConfigSectionValueJSONArray(@NonNull String name, @NonNull String section, @NonNull String key) {
        synchronized (m_syncronizationConfigs) {
            JSONObject config = m_configs.get(name);

            if (config == null) {
                return null;
            }

            JSONObject value = config.optJSONObject(section);

            if (value == null) {
                return null;
            }

            return value.optJSONArray(key);
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