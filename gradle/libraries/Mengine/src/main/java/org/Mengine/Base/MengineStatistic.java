package org.Mengine.Base;

import android.content.SharedPreferences;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import java.util.HashMap;
import java.util.Map;

public class MengineStatistic {
    public static final String TAG = "MengineStatistic";

    private static final Map<String, Object> m_statistics = new HashMap<>();

    static public void load(@NonNull MengineApplication application) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(application, TAG);

        for (Map.Entry<String, ?> entry : settings.getAll().entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();

            if (value instanceof Long) {
                m_statistics.put(key, value);
            } else if (value instanceof String) {
                try {
                    double v = Double.parseDouble((String) value);

                    m_statistics.put(key, v);
                } catch (final NumberFormatException e) {
                    MengineLog.logError(TAG, "load: %s invalid double value [%s]"
                        , key
                        , value
                    );
                }
            } else {
                MengineLog.logError(TAG, "load: %s unsupported type [%s]"
                    , key
                    , value.getClass().getName()
                );
            }
        }
    }

    static public void save(@NonNull MengineApplication application) {
        SharedPreferences settings = MenginePreferences.getPrivateSharedPreferences(application, TAG);

        SharedPreferences.Editor editor = settings.edit();

        for(Map.Entry<String, Object> entry : m_statistics.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();

            if (value instanceof Long obj_long) {
                editor.putLong(key, obj_long);
            } else if (value instanceof Double obj_double) {
                String obj_string = Double.toString(obj_double);

                editor.putString(key, obj_string);
            } else {
                MengineLog.logError(TAG, "save: %s unsupported type [%s]"
                    , key
                    , value.getClass().getName()
                );
            }
        }

        editor.apply();
    }

    static public void setInteger(@NonNull MengineApplication application, @Size(min = 1L, max = 40L) String key, long value) {
        m_statistics.put(key, value);
    }

    static public void increaseInteger(@NonNull MengineApplication application, @Size(min = 1L, max = 40L) String key, long value) {
        Object obj = m_statistics.get(key);

        if (obj == null) {
            m_statistics.put(key, value);

            return;
        }

        if (obj instanceof Long) {
            long v = (long) obj;
            v += value;

            m_statistics.put(key, v);

            return;
        }

        MengineLog.logError(TAG, "increaseInteger: %s not integer [%s]"
            , key
            , key.getClass().getName()
        );
    }

    static public void decreaseInteger(@NonNull MengineApplication application, @Size(min = 1L, max = 40L) String key, long value) {
        Object obj = m_statistics.get(key);

        if (obj == null) {
            m_statistics.put(key, -value);

            return;
        }

        if (obj instanceof Long) {
            long v = (long) obj;
            v -= value;

            m_statistics.put(key, v);
        }

        MengineLog.logError(TAG, "decreaseInteger: %s not integer [%s]"
            , key
            , key.getClass().getName()
        );
    }

    static public void increaseDouble(@NonNull MengineApplication application, @Size(min = 1L, max = 40L) String key, double value) {
        Object obj = m_statistics.get(key);

        if (obj == null) {
            m_statistics.put(key, value);

            return;
        }

        if (obj instanceof Double) {
            double v = (double) obj;
            v += value;

            m_statistics.put(key, v);

            return;
        }

        MengineLog.logError(TAG, "increaseDouble: %s not double [%s]"
            , key
            , key.getClass().getName()
        );
    }

    static public void decreaseDouble(@NonNull MengineApplication application, @Size(min = 1L, max = 40L) String key, double value) {
        Object obj = m_statistics.get(key);

        if (obj == null) {
            m_statistics.put(key, -value);

            return;
        }

        if (obj instanceof Double) {
            double v = (double) obj;
            v -= value;

            m_statistics.put(key, v);

            return;
        }

        MengineLog.logError(TAG, "decreaseDouble: %s not double [%s]"
            , key
            , key.getClass().getName()
        );
    }
}