package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdBasePoint {
    public static final String TAG = "MengineAdBasePoint";

    protected String m_name;

    protected MengineAdAttempts m_attempts;

    MengineAdBasePoint(@NonNull String name) {
        m_name = name;
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

        if ((int)value < -1) {
            MengineLog.logError(TAG, "%s attribute %s must be greater than or equal to -1"
                , m_name
                , key
            );

            return defaultValue;
        }

        return (int)value;
    }

    protected long parseAdPointTime(@NonNull JSONObject values, @NonNull String key, boolean required, long defaultValue) {
        if (values.has(key) == false) {
            if (required == true) {
                MengineLog.logError(TAG, "%s attribute %s is required"
                    , m_name
                    , key
                );
            }

            return defaultValue * 1000;
        }

        Object value = values.opt(key);

        if (value instanceof Integer == true) {
            int int_value = (int)value;

            if (int_value < -1) {
                MengineLog.logError(TAG, "%s attribute %s must be greater than or equal to -1"
                    , m_name
                    , key
                );

                return defaultValue * 1000;
            }

            return int_value * 1000;
        } else if (value instanceof Long == true) {
            long long_value = (long) value;

            if (long_value < -1) {
                MengineLog.logError(TAG, "%s attribute %s must be greater than or equal to -1"
                    , m_name
                    , key
                );

                return defaultValue * 1000;
            }

            return long_value * 1000;
        }

        MengineLog.logError(TAG, "%s attribute %s must be an integer or a long, but not a %s"
            , m_name
            , key
            , value.getClass().getSimpleName()
        );

        return defaultValue * 1000;
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
}