package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdPointBase {
    public static final MengineTag TAG = MengineTag.of("MNGAdBasePoint");

    protected String m_name;

    protected int m_id;
    protected boolean m_enabled;

    protected String m_cooldownGroupName;

    protected MengineAdAttempts m_attempts;
    protected MengineAdCooldown m_cooldown;

    protected long m_lastShowTime;

    MengineAdPointBase(@NonNull String name, @NonNull JSONObject values) {
        m_name = name;

        m_id = this.parseAdPointInteger(values, "id", false, 1);
        m_enabled = this.parseAdPointBoolean(values, "enable", true, false);

        m_cooldownGroupName = this.parseAdPointString(values, "trigger_cooldown_group", false, null);

        m_lastShowTime = 0;
    }

    public String getName() {
        return m_name;
    }

    public void setAttempts(MengineAdAttempts attempts) {
        m_attempts = attempts;
    }

    public MengineAdAttempts getAttempts() {
        return m_attempts;
    }

    public String getCooldownGroupName() {
        return m_cooldownGroupName;
    }

    public void setCooldown(MengineAdCooldown cooldown) {
        m_cooldown = cooldown;
    }

    public MengineAdCooldown getCooldown() {
        return m_cooldown;
    }

    protected boolean parseAdPointBoolean(@NonNull JSONObject values, @NonNull String key, boolean required, boolean defaultValue) {
        if (values.has(key) == false) {
            if (required == true) {
                MengineLog.logError(TAG, "%s attribute %s is required"
                    , m_name
                    , key
                );
            }

            return defaultValue;
        }

        Object value = values.opt(key);

        if (value instanceof Boolean == false) {
            MengineLog.logError(TAG, "%s attribute %s must be a boolean, but not a %s"
                , m_name
                , key
                , value.getClass().getSimpleName()
            );

            return defaultValue;
        }

        return (boolean)value;
    }

    protected int parseAdPointInteger(@NonNull JSONObject values, @NonNull String key, boolean required, int defaultValue) {
        if (values.has(key) == false) {
            if (required == true) {
                MengineLog.logError(TAG, "%s attribute %s is required"
                    , m_name
                    , key
                );
            }

            return defaultValue;
        }

        Object value = values.opt(key);

        if (value instanceof Integer == false) {
            MengineLog.logError(TAG, "%s attribute %s must be an integer, but not a %s"
                , m_name
                , key
                , value.getClass().getSimpleName()
            );

            return defaultValue;
        }

        return (int)value;
    }

    protected long parseAdPointLong(@NonNull JSONObject values, @NonNull String key, boolean required, long defaultValue) {
        if (values.has(key) == false) {
            if (required == true) {
                MengineLog.logError(TAG, "%s attribute %s is required"
                    , m_name
                    , key
                );
            }

            return defaultValue;
        }

        Object value = values.opt(key);

        if (value instanceof Integer == true) {
            int int_value = (int)value;

            return int_value;
        } else if (value instanceof Long == true) {
            long long_value = (long)value;

            return long_value;
        }

        MengineLog.logError(TAG, "%s attribute %s must be an integer or a long, but not a %s"
            , m_name
            , key
            , value.getClass().getSimpleName()
        );

        return defaultValue;
    }

    protected long parseAdPointTime(@NonNull JSONObject values, @NonNull String key, boolean required, long defaultValue) {
        long value = this.parseAdPointLong(values, key, required, defaultValue);

        long time = value * 1000;

        return time;
    }

    protected String parseAdPointString(@NonNull JSONObject values, @NonNull String key, boolean required, String defaultValue) {
        if (values.has(key) == false) {
            if (required == true) {
                MengineLog.logError(TAG, "%s attribute %s is required"
                    , m_name
                    , key
                );
            }

            return defaultValue;
        }

        Object value = values.opt(key);

        if (value instanceof String == false && value != null) {
            MengineLog.logError(TAG, "%s attribute %s must be a string or null, but not a %s"
                , m_name
                , key
                , value.getClass().getSimpleName()
            );

            return defaultValue;
        }

        return (String)value;
    }

    public void showAd() {
        m_lastShowTime = MengineUtils.getTimestamp();

        m_cooldown.resetShownTimestamp();
    }
}