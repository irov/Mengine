package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineAdFrequency {
    public static final int SAVE_VERSION = 1;

    protected long m_startTimestamp = 0;
    protected long m_showCount = 0;

    public synchronized MengineBundle onSave(@NonNull MengineApplication application) {
        MengineBundle bundle = new MengineBundle();

        bundle.putInt("version", SAVE_VERSION);
        bundle.putLong("start", m_startTimestamp);
        bundle.putLong("count", m_showCount);

        return bundle;
    }

    public synchronized void onLoad(@NonNull MengineApplication application, @NonNull MengineBundle bundle) {
        m_startTimestamp = bundle.getLong("start", 0L);
        m_showCount = bundle.getLong("count", 0L);
    }

    public synchronized boolean hasShowQuota(int showLimit, long showTimeout) {
        if (showLimit < 0) {
            return true;
        }

        long count = m_showCount;
        long timestamp = MengineUtils.getTimestamp();

        if (timestamp - m_startTimestamp >= showTimeout) {
            count = 0;
        }

        if (count >= showLimit) {
            return false;
        }

        return true;
    }

    public synchronized void recordShown(long showTimeout) {
        long timestamp = MengineUtils.getTimestamp();

        if (m_showCount == 0 || timestamp - m_startTimestamp >= showTimeout) {
            m_startTimestamp = timestamp;
            m_showCount = 0;
        }

        m_showCount += 1;
    }
}
