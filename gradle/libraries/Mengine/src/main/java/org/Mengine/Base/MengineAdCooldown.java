package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineAdCooldown {
    public static final int SAVE_VERSION = 1;

    private volatile long m_timestamp;

    MengineAdCooldown() {
        m_timestamp = 0;
    }

    public MengineBundle onSave(@NonNull MengineApplication application) {
        MengineBundle bundle = new MengineBundle();

        bundle.putInt("version", SAVE_VERSION);
        bundle.putLong("timestamp", m_timestamp);

        return bundle;
    }

    public void onLoad(@NonNull MengineApplication application, @NonNull MengineBundle bundle) {
        int version = bundle.getInt("version", 0);

        m_timestamp = bundle.getLong("timestamp", 0L);
    }

    public long getLastShownTimestamp() {
        return m_timestamp;
    }

    public void resetShownTimestamp() {
        long timestamp = MengineUtils.getTimestamp();

        m_timestamp = timestamp;
    }
}