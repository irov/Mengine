package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

public class MengineStatistic {
    public static final MengineTag TAG = MengineTag.of("MNGStatistic");

    private static final Bundle STATISTICS = new Bundle();
    private static final Object STATISTICS_LOCK = new Object();

    static public void load(@NonNull MengineApplication application) {
        synchronized (MengineStatistic.STATISTICS_LOCK) {
            MenginePreferences.copyPreferenceBundle("statistics", MengineStatistic.STATISTICS);
        }
    }

    static public void save(@NonNull MengineApplication application) {
        synchronized (MengineStatistic.STATISTICS_LOCK) {
            MenginePreferences.setPreferenceBundle("statistics", MengineStatistic.STATISTICS);
        }
    }

    static public void setInteger(@Size(min = 1L, max = 40L) String key, long value) {
        synchronized (MengineStatistic.STATISTICS_LOCK) {
            MengineStatistic.STATISTICS.putLong(key, value);
        }
    }

    static public long getInteger(@Size(min = 1L, max = 40L) String key, long defaultValue) {
        synchronized (MengineStatistic.STATISTICS_LOCK) {
            return MengineStatistic.STATISTICS.getLong(key, defaultValue);
        }
    }

    static public void increaseInteger(@Size(min = 1L, max = 40L) String key, long value) {
        synchronized (MengineStatistic.STATISTICS_LOCK) {
            long v = MengineStatistic.STATISTICS.getLong(key, 0L);
            v += value;
            MengineStatistic.STATISTICS.putLong(key, v);
        }
    }

    static public void decreaseInteger(@Size(min = 1L, max = 40L) String key, long value) {
        synchronized (MengineStatistic.STATISTICS_LOCK) {
            long v = MengineStatistic.STATISTICS.getLong(key, 0L);
            v -= value;
            MengineStatistic.STATISTICS.putLong(key, v);
        }
    }

    static public void setDouble(@Size(min = 1L, max = 40L) String key, double value) {
        synchronized (MengineStatistic.STATISTICS_LOCK) {
            MengineStatistic.STATISTICS.putDouble(key, value);
        }
    }

    static public double getDouble(@Size(min = 1L, max = 40L) String key, double defaultValue) {
        synchronized (MengineStatistic.STATISTICS_LOCK) {
            return MengineStatistic.STATISTICS.getDouble(key, defaultValue);
        }
    }

    static public void increaseDouble(@Size(min = 1L, max = 40L) String key, double value) {
        synchronized (MengineStatistic.STATISTICS_LOCK) {
            double v = MengineStatistic.STATISTICS.getDouble(key, 0.0);
            v += value;
            MengineStatistic.STATISTICS.putDouble(key, v);
        }
    }

    static public void decreaseDouble(@Size(min = 1L, max = 40L) String key, double value) {
        synchronized (MengineStatistic.STATISTICS_LOCK) {
            double v = MengineStatistic.STATISTICS.getDouble(key, 0.0);
            v -= value;
            MengineStatistic.STATISTICS.putDouble(key, v);
        }
    }
}