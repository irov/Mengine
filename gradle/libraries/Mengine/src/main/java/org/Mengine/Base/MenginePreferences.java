package org.Mengine.Base;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MenginePreferences {
    public static final String TAG = "MenginePreferences";

    static public SharedPreferences getPrivateSharedPreferences(@NonNull Context context, @NonNull String tag) {
        String packageName = context.getPackageName();

        SharedPreferences settings = context.getSharedPreferences(packageName + "." + tag, Context.MODE_PRIVATE);

        return settings;
    }

    static public boolean hasPreference(@NonNull Context context, @NonNull String tag, @NonNull String name) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        boolean has = settings.contains(name);

        return has;
    }

    static public boolean getPreferenceBoolean(@NonNull Context context, @NonNull String tag, @NonNull String name, boolean defaultValue) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        boolean value = settings.getBoolean(name, defaultValue);

        return value;
    }

    static public void setPreferenceBoolean(@NonNull Context context, @NonNull String tag, @NonNull String name, boolean value) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean(name, value);
        editor.apply();
    }

    static public long getPreferenceInteger(@NonNull Context context, @NonNull String tag, @NonNull String name, long defaultValue) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        long value = settings.getLong(name, defaultValue);

        return value;
    }

    static public void setPreferenceInteger(@NonNull Context context, @NonNull String tag, @NonNull String name, long value) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        SharedPreferences.Editor editor = settings.edit();
        editor.putLong(name, value);
        editor.apply();
    }

    static public String getPreferenceString(@NonNull Context context, @NonNull String tag, @NonNull String name, String defaultValue) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        String value = settings.getString(name, defaultValue);

        return value;
    }

    static public void setPreferenceString(@NonNull Context context, @NonNull String tag, @NonNull String name, String value) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        SharedPreferences.Editor editor = settings.edit();
        editor.putString(name, value);
        editor.apply();
    }

    static public <T extends Enum<T>> T getPreferenceEnum(@NonNull Context context, @NonNull String tag, @NonNull String name, Class<T> enumType, T defaultValue) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        String value = settings.getString(name, null);

        if (value == null) {
            return defaultValue;
        }

        try {
            T enumValue = Enum.valueOf(enumType, value);

            return enumValue;
        } catch (final IllegalArgumentException e) {
            MengineLog.logError(TAG, "invalid get preference '%s' enum: %s e: %s"
                , tag
                , name
                , e.getMessage()
            );

            return defaultValue;
        }
    }

    static public void setPreferenceEnum(@NonNull Context context, @NonNull String tag, @NonNull String name, Enum<?> value) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        SharedPreferences.Editor editor = settings.edit();
        editor.putString(name, value.name());
        editor.apply();
    }

    static public Bundle getPreferenceBundle(@NonNull Context context, @NonNull String tag, @NonNull String name) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        String value = settings.getString(name, null);

        if (value == null) {
            return null;
        }

        try {
            JSONObject json = new JSONObject(value);

            Bundle bundle = MengineUtils.bundleFromJSONObject(json);

            return bundle;
        } catch (final org.json.JSONException e) {
            MengineLog.logError(TAG, "invalid get preference '%s' bundle: %s e: %s"
                , tag
                , name
                , e.getMessage()
            );

            return null;
        }
    }

    static public void setPreferenceBundle(@NonNull Context context, @NonNull String tag, @NonNull String name, Bundle value) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        JSONObject json = MengineUtils.jsonObjectFromBundle(value);

        SharedPreferences.Editor editor = settings.edit();
        editor.putString(name, json.toString());
        editor.apply();
    }

    static public JSONObject getPreferenceJSON(@NonNull Context context, @NonNull String tag, @NonNull String name) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        String value = settings.getString(name, null);

        if (value == null) {
            return null;
        }

        try {
            JSONObject json = new JSONObject(value);

            return json;
        } catch (final org.json.JSONException e) {
            MengineLog.logError(TAG, "invalid get preference '%s' json: %s e: %s"
                , tag
                , name
                , e.getMessage()
            );

            return null;
        }
    }

    static public void setPreferenceJSON(@NonNull Context context, @NonNull String tag, @NonNull String name, JSONObject value) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        SharedPreferences.Editor editor = settings.edit();
        editor.putString(name, value.toString());
        editor.apply();
    }

    static public void removePreference(@NonNull Context context, @NonNull String tag, @NonNull String name) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        SharedPreferences.Editor editor = settings.edit();
        editor.remove(name);
        editor.apply();
    }

    static public void clearPreferences(@NonNull Context context, @NonNull String tag) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(context, tag);

        SharedPreferences.Editor editor = settings.edit();
        editor.clear();
        editor.apply();
    }
}