package org.Mengine.Base;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;

import androidx.annotation.NonNull;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Collection;
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

            MenginePreferences.SETTINGS.put(key, value);
        }
    }

    static public boolean hasPreference(@NonNull String name) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            boolean has = MenginePreferences.SETTINGS.containsValue(name);

            return has;
        }
    }

    static public boolean getPreferenceBoolean(@NonNull String name, boolean defaultValue) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Boolean value = (Boolean)MenginePreferences.SETTINGS.getOrDefault(name, Boolean.valueOf(defaultValue));

            return value;
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

    static public long getPreferenceLong(@NonNull String name, long defaultValue) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Long value = (Long)MenginePreferences.SETTINGS.getOrDefault(name, Long.valueOf(defaultValue));

            return value;
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

    static public Set<String> getPreferenceStrings(@NonNull String name, Set<String> defaultValue) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Object v = MenginePreferences.SETTINGS.getOrDefault(name, defaultValue);

            Set<String> value = (Set<String>)MenginePreferences.SETTINGS.getOrDefault(name, defaultValue);

            return value;
        }
    }

    static public void setPreferenceStrings(@NonNull String name, Set<String> value) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Set<String> currentValue = (Set<String>)MenginePreferences.SETTINGS.getOrDefault(name, null);

            if (Objects.equals(currentValue, value) == true) {
                return;
            }

            MenginePreferences.SETTINGS.put(name, value);
        }

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        editor.putStringSet(name, value);
        editor.apply();
    }

    static public void addPreferenceStrings(@NonNull String name, Collection<String> value) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            Set<String> currentValue = (Set<String>)MenginePreferences.SETTINGS.getOrDefault(name, null);

            if (currentValue == null) {
                currentValue = new HashSet<>();
                MenginePreferences.SETTINGS.put(name, currentValue);
            }

            currentValue.addAll(value);
        }

        Set<String> values = MenginePreferences.getPreferenceStrings(name, null);

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        editor.putStringSet(name, values);
        editor.apply();
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

    static public Bundle getPreferenceBundle(@NonNull String name, Supplier<Bundle> defaultValue) {
        String value = MenginePreferences.getPreferenceString(name, null);

        if (value == null) {
            return defaultValue != null ? defaultValue.get() : null;
        }

        try {
            JSONObject json = new JSONObject(value);

            Bundle bundle = MengineUtils.bundleFromJSONObject(json);

            return bundle;
        } catch (final JSONException e) {
            MengineLog.logError(TAG, "invalid get preference: %s bundle: %s e: %s"
                , MenginePreferences.NAME
                , name
                , e.getMessage()
            );

            return defaultValue != null ? defaultValue.get() : null;
        }
    }

    static public void copyPreferenceBundle(@NonNull String name, @NonNull Bundle bundle) {
        String value = MenginePreferences.getPreferenceString(name, null);

        if (value == null) {
            return;
        }

        try {
            JSONObject json = new JSONObject(value);

            MengineUtils.copyBundleFromJSONObject(bundle, json);
        } catch (final JSONException e) {
            MengineLog.logError(TAG, "invalid copy preference: %s bundle: %s e: %s"
                , MenginePreferences.NAME
                , name
                , e.getMessage()
            );
        }
    }

    static public void setPreferenceBundle(@NonNull String name, Bundle value) {
        JSONObject json = MengineUtils.jsonObjectFromBundle(value);

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
        String jsonString = value.toString();

        MenginePreferences.setPreferenceString(name, jsonString);
    }

    static public void setPreferences(@NonNull Map<String, ?> preferences) {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            MenginePreferences.SETTINGS.putAll(preferences);
        }

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        for (Map.Entry<String, ?> entry : preferences.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();

            if (value instanceof Boolean) {
                editor.putBoolean(key, (Boolean) value);
            } else if (value instanceof Long) {
                editor.putLong(key, (Long) value);
            } else if (value instanceof String) {
                editor.putString(key, (String) value);
            } else {
                MengineLog.logError(TAG, "unsupported preference type: %s key: %s value: %s"
                    , MenginePreferences.NAME
                    , key
                    , value
                );
            }
        }
        editor.apply();
    }

    static public void setPreferences(@NonNull Bundle preferences) {
        Map<String, Object> preferencesMap = new HashMap<>();

        for (String key : preferences.keySet()) {
            Object value = preferences.get(key);
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

    static public void clearPreferences() {
        synchronized (MenginePreferences.SETTINGS_SYNC) {
            MenginePreferences.SETTINGS.clear();
        }

        SharedPreferences.Editor editor = MenginePreferences.PREFERENCES.edit();
        editor.clear();
        editor.apply();
    }
}