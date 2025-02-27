package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;

public class MengineAdAttempts {
    public static final int SAVE_VERSION = 1;

    public int m_attempts = 0;

    public Bundle onSave(@NonNull MengineApplication application) {
        Bundle bundle = new Bundle();

        bundle.putInt("version", SAVE_VERSION);
        bundle.putInt("attempts", m_attempts);

        return bundle;
    }

    public void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
        int version = bundle.getInt("version", 0);
        m_attempts = bundle.getInt("attempts", 0);
    }

    public long getAttempts() {
        return m_attempts;
    }

    public int attempt() {
        m_attempts += 1;

        return m_attempts;
    }
}