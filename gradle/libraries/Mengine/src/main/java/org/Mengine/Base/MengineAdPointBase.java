package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import org.json.JSONObject;

public class MengineAdPointBase {
    public static final MengineTag TAG = MengineTag.of("MNGAdBasePoint");

    protected final String m_name;

    protected final int m_id;
    protected final boolean m_enabled;

    protected final String m_cooldownGroupName;

    protected MengineAdAttempts m_attempts;
    protected MengineAdCooldown m_cooldown;

    protected long m_lastShowTime;

    MengineAdPointBase(@NonNull String name, @NonNull JSONObject values) {
        m_name = name;

        m_id = this.parseAdPointInteger(values, "id", false, 1, null, null);
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

    // Bounds are inclusive; null disables the corresponding bound.
    protected int parseAdPointInteger(@NonNull JSONObject values, @NonNull String key, boolean required, int defaultValue, @Nullable Integer minValue, @Nullable Integer maxValue) {
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

        int result = (int)value;

        if (minValue != null && result < minValue) {
            MengineLog.logError(TAG, "%s attribute %s must be >= %d, but got %d; using default %d"
                , m_name
                , key
                , minValue
                , result
                , defaultValue
            );

            return defaultValue;
        }

        if (maxValue != null && result > maxValue) {
            MengineLog.logError(TAG, "%s attribute %s must be <= %d, but got %d; using default %d"
                , m_name
                , key
                , maxValue
                , result
                , defaultValue
            );

            return defaultValue;
        }

        return result;
    }

    protected long parseAdPointLong(@NonNull JSONObject values, @NonNull String key, boolean required, long defaultValue, @Nullable Long minValue, @Nullable Long maxValue) {
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
        long result;

        if (value instanceof Integer == true) {
            result = (int)value;
        } else if (value instanceof Long == true) {
            result = (long)value;
        } else {
            Class<?> valueClass = value.getClass();
            String valueClassName = valueClass.getSimpleName();

            MengineLog.logError(TAG, "%s attribute %s must be an integer or a long, but not a %s"
                , m_name
                , key
                , valueClassName
            );

            return defaultValue;
        }

        if (minValue != null && result < minValue) {
            MengineLog.logError(TAG, "%s attribute %s must be >= %d, but got %d; using default %d"
                , m_name
                , key
                , minValue
                , result
                , defaultValue
            );

            return defaultValue;
        }

        if (maxValue != null && result > maxValue) {
            MengineLog.logError(TAG, "%s attribute %s must be <= %d, but got %d; using default %d"
                , m_name
                , key
                , maxValue
                , result
                , defaultValue
            );

            return defaultValue;
        }

        return result;
    }

    // JSON, default value and bounds are seconds; the result is milliseconds.
    protected long parseAdPointTime(@NonNull JSONObject values, @NonNull String key, boolean required, long defaultValue, @Nullable Long minValue, @Nullable Long maxValue) {
        long value = this.parseAdPointLong(values, key, required, defaultValue, minValue, maxValue);

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

    public boolean isEnabled() {
        return m_enabled;
    }

    public long getLastShowTime() {
        return m_lastShowTime;
    }

    public void showAd() {
        m_lastShowTime = MengineUtils.getTimestamp();

        m_cooldown.resetShownTimestamp();
    }
}