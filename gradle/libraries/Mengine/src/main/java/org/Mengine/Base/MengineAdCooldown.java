package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;

public class MengineAdCooldown {
    public long m_timestamp;

    MengineAdCooldown() {
        m_timestamp = 0;
    }

    public void onLoad(@NonNull MengineApplication application, Bundle bundle) {
        m_timestamp = bundle.getLong("timestamp", 0);
    }

    public Bundle onSave(@NonNull MengineApplication application) {
        Bundle bundle = new Bundle();

        bundle.putLong("timestamp", m_timestamp);

        return bundle;
    }

    public long getLastShownTimestamp() {
        return m_timestamp;
    }

    public void resetShownTimestamp() {
        long timestamp = MengineUtils.getTimestamp();

        m_timestamp = timestamp;
    }
}