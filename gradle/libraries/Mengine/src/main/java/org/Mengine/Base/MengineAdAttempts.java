package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineAdAttempts {
    public static final int SAVE_VERSION = 1;

    public int m_attempts = 0;

    public MengineBundle onSave(@NonNull MengineApplication application) {
        MengineBundle bundle = new MengineBundle();

        bundle.putInt("version", SAVE_VERSION);
        bundle.putInt("attempts", m_attempts);

        return bundle;
    }

    public void onLoad(@NonNull MengineApplication application, @NonNull MengineBundle bundle) {
        int version = bundle.getInt("version", 0);

        m_attempts = bundle.getInt("attempts", 0);
    }

    public int getAttempts() {
        return m_attempts;
    }

    public int incrementAttempts() {
        m_attempts += 1;

        return m_attempts;
    }
}