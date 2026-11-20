package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Iterator;
import java.util.Map;

public final class MengineBundle {
    private static final MengineTag TAG = MengineTag.of("MNGBundle");

    private final JSONObject m_values;

    public MengineBundle() {
        this(new JSONObject());
    }

    MengineBundle(@NonNull JSONObject values) {
        m_values = values;
    }

    @NonNull
    JSONObject getJSONObject() {
        return m_values;
    }

    public boolean containsKey(@NonNull String name) {
        if (m_values.has(name) == false) {
            return false;
        }

        return true;
    }

    @NonNull
    public Iterable<String> keys() {
        Iterable<String> keys = m_values::keys;

        return keys;
    }

    @Nullable
    Object getValue(@NonNull String name) {
        Object value = m_values.opt(name);

        if (value == JSONObject.NULL) {
            return null;
        }

        return value;
    }

    private void putValue(@NonNull String name, @Nullable Object value) {
        try {
            m_values.put(name, value);
        } catch (final JSONException e) {
            MengineLog.logException(TAG, e, Map.of("key", name));
        }
    }

    void putAll(@NonNull JSONObject values) {
        Iterator<String> keys = values.keys();

        while (keys.hasNext() == true) {
            String key = keys.next();
            Object value = values.opt(key);

            if (value == JSONObject.NULL) {
                continue;
            }

            this.putValue(key, value);
        }
    }

    public void putBoolean(@NonNull String name, boolean value) {
        this.putValue(name, value);
    }

    public boolean getBoolean(@NonNull String name, boolean defaultValue) {
        Object value = m_values.opt(name);

        if (value instanceof Boolean booleanValue) {
            return booleanValue;
        }

        return defaultValue;
    }

    public void putInt(@NonNull String name, int value) {
        this.putValue(name, value);
    }

    public int getInt(@NonNull String name, int defaultValue) {
        long value = this.getLong(name, defaultValue);

        if (value < Integer.MIN_VALUE) {
            return defaultValue;
        }

        if (value > Integer.MAX_VALUE) {
            return defaultValue;
        }

        return (int)value;
    }

    public void putLong(@NonNull String name, long value) {
        this.putValue(name, value);
    }

    public long getLong(@NonNull String name, long defaultValue) {
        Object value = m_values.opt(name);
        long longValue = MengineBundle.getLongValue(value, defaultValue);

        return longValue;
    }

    static long getLongValue(@Nullable Object value, long defaultValue) {
        if (value instanceof Long longValue) {
            return longValue;
        }

        // JSON numbers in the int range are parsed as Integer.
        if (value instanceof Integer integerValue) {
            return integerValue;
        }

        return defaultValue;
    }

    public void putDouble(@NonNull String name, double value) {
        this.putValue(name, value);
    }

    public double getDouble(@NonNull String name, double defaultValue) {
        Object value = m_values.opt(name);

        if (value instanceof Number number) {
            double doubleValue = number.doubleValue();

            return doubleValue;
        }

        return defaultValue;
    }

    public void putString(@NonNull String name, @Nullable String value) {
        this.putValue(name, value);
    }

    @Nullable
    public String getString(@NonNull String name, @Nullable String defaultValue) {
        Object value = m_values.opt(name);

        if (value instanceof String stringValue) {
            return stringValue;
        }

        return defaultValue;
    }

    public void putBundle(@NonNull String name, @NonNull MengineBundle value) {
        this.putValue(name, value.m_values);
    }

    @Nullable
    public MengineBundle getBundle(@NonNull String name) {
        JSONObject values = m_values.optJSONObject(name);

        if (values == null) {
            return null;
        }

        MengineBundle bundle = new MengineBundle(values);

        return bundle;
    }

    @NonNull
    @Override
    public String toString() {
        String json = MengineUtils.jsonStringFromJSONObject(m_values);

        if (json == null) {
            return "{}";
        }

        return json;
    }
}
