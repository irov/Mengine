package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.core.os.BundleCompat;

public class MengineAdFrequency {
    public static final int SAVE_VERSION = 1;

    protected long m_startTimestamp = 0;
    protected long m_showCount = 0;

    public synchronized Bundle onSave(@NonNull MengineApplication application) {
        Bundle bundle = new Bundle();

        bundle.putInt("version", SAVE_VERSION);
        bundle.putLong("start", m_startTimestamp);
        bundle.putLong("count", m_showCount);

        return bundle;
    }

    public synchronized void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
        // JSON-backed bundles may restore small long values as Integer.
        Number start = BundleCompat.getSerializable(bundle, "start", Number.class);
        Number count = BundleCompat.getSerializable(bundle, "count", Number.class);

        m_startTimestamp = start != null ? start.longValue() : 0;
        m_showCount = count != null ? count.longValue() : 0;
    }

    public synchronized boolean hasShowQuota(int showLimit, long showTimeout) {
        if (showLimit < 0) {
            return true;
        }

        long count = m_showCount;
        long now = MengineUtils.getTimestamp();

        if (count == 0 || now - m_startTimestamp >= showTimeout) {
            count = 0;
        }

        return count < showLimit;
    }

    public synchronized void recordShown(long showTimeout) {
        long now = MengineUtils.getTimestamp();

        if (m_showCount == 0 || now - m_startTimestamp >= showTimeout) {
            m_startTimestamp = now;
            m_showCount = 0;
        }

        m_showCount += 1;
    }
}
