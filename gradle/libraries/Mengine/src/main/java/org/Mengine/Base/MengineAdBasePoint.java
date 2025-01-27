package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdBasePoint {
    public static final String TAG = "AdBasePoint";

    public String m_name;

    MengineAdBasePoint(@NonNull String name) {
        m_name = name;
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

        Object id = values.opt(key);

        if (id instanceof Boolean == false) {
            MengineLog.logError(TAG, "%s attribute %s must be a boolean, but not a %s"
                , m_name
                , key
                , id.getClass().getSimpleName()
            );

            return defaultValue;
        }

        return (boolean)id;
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

        Object id = values.opt(key);

        if (id instanceof Integer == false) {
            MengineLog.logError(TAG, "%s attribute %s must be an integer, but not a %s"
                , m_name
                , key
                , id.getClass().getSimpleName()
            );

            return defaultValue;
        }

        if ((int)id < -1) {
            MengineLog.logError(TAG, "%s attribute %s must be greater than or equal to -1"
                , m_name
                , key
            );

            return defaultValue;
        }

        return (int)id;
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

        Object id = values.opt(key);

        if (id instanceof Integer == false && id instanceof Long == false) {
            MengineLog.logError(TAG, "%s attribute %s must be an integer or a long, but not a %s"
                , m_name
                , key
                , id.getClass().getSimpleName()
            );

            return defaultValue;
        }

        if ((long)id < -1) {
            MengineLog.logError(TAG, "%s attribute %s must be greater than or equal to -1"
                , m_name
                , key
            );

            return defaultValue;
        }

        return (long)id;
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

        Object id = values.opt(key);

        if (id instanceof String == false) {
            MengineLog.logError(TAG, "%s attribute %s must be a string, but not a %s"
                , m_name
                , key
                , id.getClass().getSimpleName()
            );

            return defaultValue;
        }

        return (String)id;
    }
}