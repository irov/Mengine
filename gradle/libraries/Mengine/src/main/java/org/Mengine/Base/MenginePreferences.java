package org.Mengine.Base;

import android.content.Context;
import android.content.SharedPreferences;

import androidx.annotation.NonNull;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;
import java.util.function.Supplier;
import java.util.HashMap;
import java.util.Map;

public class MenginePreferences {
    public static final MengineTag TAG = MengineTag.of("MNGPreferences");

    protected static String NAME;
    protected static SharedPreferences PREFERENCES;
    protected static final Map<String, Object> SETTINGS = new HashMap<>();
    protected static final Object SETTINGS_SYNC = new Object();

    static public void initialize(@NonNull Context context, @NonNull MengineTag tag) {
        String packageName = context.getPackageName();

        MenginePreferences.NAME = packageName + "." + tag;
        MenginePreferences.PREFERENCES = context.getSharedPreferences(MenginePreferences.NAME, Context.MODE_PRIVATE);

        Map<String, ?> settings = MenginePreferences.PREFERENCES.getAll();

        for (Map.Entry<String, ?> entry : settings.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();

            if (value instanceof Set<?> setValue) {
                value = Collections.unmodifiableSet(new HashSet<>(setValue));
            }

            MenginePreferences.SETTINGS.put(key, value);
        }
    }

    static public boolean hasPreference(@NonNull String name) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            boolean has = MenginePreferences.SETTINGS.containsKey(name);

            return has;
        }
    }

    static public boolean getPreferenceBoolean(@NonNull String name, boolean defaultValue) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Boolean value = (Boolean)MenginePreferences.SETTINGS.getOrDefault(name, defaultValue);

            return Boolean.TRUE.equals(value);
        }
    }

    static public void setPreferenceBoolean(@NonNull String name, boolean value) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Boolean currentValue = (Boolean)MenginePreferences.SETTINGS.getOrDefault(name, null);

            if (Objects.equals(currentValue, value) == true) {
                return;
            }

            MenginePreferences.SETTINGS.put(name, value);
        }

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        editor.putBoolean(name, value);
        editor.apply();
    }

    static public boolean setPreferenceBooleanImmediately(@NonNull String name, boolean value) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            MenginePreferences.SETTINGS.put(name, value);
        }

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        editor.putBoolean(name, value);

        return editor.commit();
    }

    static public long getPreferenceLong(@NonNull String name, long defaultValue) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Long value = (Long)MenginePreferences.SETTINGS.getOrDefault(name, defaultValue);

            return value.longValue();
        }
    }

    static public void setPreferenceLong(@NonNull String name, long value) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Long currentValue = (Long)MenginePreferences.SETTINGS.getOrDefault(name, null);

            if (Objects.equals(currentValue, value) == true) {
                return;
            }

            MenginePreferences.SETTINGS.put(name, value);
        }

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        editor.putLong(name, value);
        editor.apply();
    }

    static public long addPreferenceLong(@NonNull String name, long value) {
        long newValue;
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            long currentValue = (Long)MenginePreferences.SETTINGS.getOrDefault(name, 0L);

            if (value == 0L) {
                return currentValue;
            }

            newValue = currentValue + value;

            MenginePreferences.SETTINGS.put(name, newValue);
        }

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        editor.putLong(name, newValue);
        editor.apply();

        return newValue;
    }

    static public String getPreferenceString(@NonNull String name, String defaultValue) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            String value = (String)MenginePreferences.SETTINGS.getOrDefault(name, defaultValue);

            return value;
        }
    }

    static public void setPreferenceString(@NonNull String name, String value) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            String currentValue = (String)MenginePreferences.SETTINGS.getOrDefault(name, null);

            if (Objects.equals(currentValue, value) == true) {
                return;
            }

            MenginePreferences.SETTINGS.put(name, value);
        }

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        editor.putString(name, value);
        editor.apply();
    }

    @SuppressWarnings("unchecked")
    static public Set<String> getPreferenceStrings(@NonNull String name, Set<String> defaultValue) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Set<String> value = (Set<String>)MenginePreferences.SETTINGS.getOrDefault(name, defaultValue);

            return value;
        }
    }

    @SuppressWarnings("unchecked")
    static public void setPreferenceStrings(@NonNull String name, Set<String> value) {
        Set<String> strings = null;

        if (value != null) {
            strings = Collections.unmodifiableSet(new HashSet<>(value));
        }

        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Set<String> currentValue = (Set<String>)MenginePreferences.SETTINGS.getOrDefault(name, null);

            if (Objects.equals(currentValue, strings) == true) {
                return;
            }

            MenginePreferences.SETTINGS.put(name, strings);

            SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
            editor.putStringSet(name, strings);
            editor.apply();
        }
    }

    @SuppressWarnings("unchecked")
    static public void addPreferenceStrings(@NonNull String name, Collection<String> value) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Set<String> currentValue = (Set<String>)MenginePreferences.SETTINGS.getOrDefault(name, null);

            Set<String> newValue = new HashSet<>();

            if (currentValue != null) {
                newValue.addAll(currentValue);
            }

            newValue.addAll(value);

            Set<String> strings = Collections.unmodifiableSet(newValue);

            MenginePreferences.SETTINGS.put(name, strings);

            SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
            editor.putStringSet(name, strings);
            editor.apply();
        }
    }

    @SuppressWarnings("unchecked")
    static public void removePreferenceStrings(@NonNull String name, Collection<String> value) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Set<String> currentValue = (Set<String>)MenginePreferences.SETTINGS.getOrDefault(name, null);

            if (currentValue == null) {
                return;
            }

            Set<String> newValue = new HashSet<>(currentValue);

            if (newValue.removeAll(value) == false) {
                return;
            }

            Set<String> strings = Collections.unmodifiableSet(newValue);

            MenginePreferences.SETTINGS.put(name, strings);

            SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
            editor.putStringSet(name, strings);
            editor.apply();
        }
    }

    static public <T extends Enum<T>> T getPreferenceEnum(@NonNull String name, Class<T> enumType, T defaultValue) {
        String value = MenginePreferences.getPreferenceString(name, null);

        if (value == null) {
            return defaultValue;
        }

        try {
            T enumValue = Enum.valueOf(enumType, value);

            return enumValue;
        } catch (final IllegalArgumentException e) {
            MengineLog.logError(TAG, "invalid get preference: %s enum: %s e: %s"
                , MenginePreferences.NAME
                , name
                , e.getMessage()
            );

            return defaultValue;
        }
    }

    static public void setPreferenceEnum(@NonNull String name, Enum<?> value) {
        String valueName = value.name();

        MenginePreferences.setPreferenceString(name, valueName);
    }

    static public MengineBundle getPreferenceBundle(@NonNull String name, Supplier<MengineBundle> defaultValue) {
        JSONObject json = MenginePreferences.getPreferenceJSON(name, null);

        if (json == null) {
            return defaultValue != null ? defaultValue.get() : null;
        }

        MengineBundle bundle = new MengineBundle(json);

        return bundle;
    }

    static public void copyPreferenceBundle(@NonNull String name, @NonNull MengineBundle bundle) {
        JSONObject json = MenginePreferences.getPreferenceJSON(name, null);

        if (json == null) {
            return;
        }

        bundle.putAll(json);
    }

    static public void setPreferenceBundle(@NonNull String name, @NonNull MengineBundle value) {
        JSONObject json = value.getJSONObject();

        MenginePreferences.setPreferenceJSON(name, json);
    }

    static public JSONObject getPreferenceJSON(@NonNull String name, Supplier<? extends JSONObject> defaultValue) {
        String value = MenginePreferences.getPreferenceString(name, null);

        if (value == null) {
            return defaultValue != null ? defaultValue.get() : null;
        }

        try {
            JSONObject json = new JSONObject(value);

            return json;
        } catch (final JSONException e) {
            MengineLog.logError(TAG, "invalid get preference: %s json: %s e: %s"
                , MenginePreferences.NAME
                , name
                , e.getMessage()
            );

            return defaultValue != null ? defaultValue.get() : null;
        }
    }

    static public void setPreferenceJSON(@NonNull String name, JSONObject value) {
        String jsonString = MengineUtils.jsonStringFromJSONObject(value);

        if (jsonString == null) {
            return;
        }

        MenginePreferences.setPreferenceString(name, jsonString);
    }

    static public void setPreferences(@NonNull Map<String, ?> preferences) {
        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();

        synchronized (MenginePreferences.SETTINGS_SYNC) {
            for (Map.Entry<String, ?> entry : preferences.entrySet()) {
                String key = entry.getKey();
                Object value = entry.getValue();

                if (value instanceof Integer integerValue) {
                    value = integerValue.longValue();
                }

                if (value instanceof Boolean booleanValue) {
                    editor.putBoolean(key, booleanValue);
                } else if (value instanceof Long longValue) {
                    editor.putLong(key, longValue);
                } else if (value instanceof String stringValue) {
                    editor.putString(key, stringValue);
                } else {
                    MengineLog.logError(TAG, "unsupported preference type: %s key: %s value: %s"
                        , MenginePreferences.NAME
                        , key
                        , value
                    );

                    continue;
                }

                MenginePreferences.SETTINGS.put(key, value);
            }

            editor.apply();
        }
    }

    static public void setPreferences(@NonNull MengineBundle preferences) {
        Map<String, Object> preferencesMap = new HashMap<>();

        for (String key : preferences.keys()) {
            Object value = preferences.getValue(key);
            preferencesMap.put(key, value);
        }

        MenginePreferences.setPreferences(preferencesMap);
    }

    static public void removePreference(@NonNull String name) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            MenginePreferences.SETTINGS.remove(name);
        }

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        editor.remove(name);
        editor.apply();
    }

    static public boolean removePreferenceImmediately(@NonNull String name) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            MenginePreferences.SETTINGS.remove(name);
        }

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        editor.remove(name);

        return editor.commit();
    }

    static public void clearPreferences() {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            MenginePreferences.SETTINGS.clear();
        }

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        editor.clear();
        editor.apply();
    }
}
