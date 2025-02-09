package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;

public class MengineAdAttempts {
    public int m_attempts = 0;

    public void onLoad(@NonNull MengineApplication application, Bundle bundle) {
        m_attempts = bundle.getInt("attempts", 0);
    }

    public Bundle onSave(@NonNull MengineApplication application) {
        Bundle bundle = new Bundle();

        bundle.putInt("attempts", m_attempts);

        return bundle;
    }

    public long getAttempts() {
        return m_attempts;
    }

    public int attempt() {
        m_attempts += 1;

        return m_attempts;
    }
}