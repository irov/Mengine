package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

public class MengineFragmentAttribution extends MengineFragment<MengineListenerAttribution> {
    public static MengineFragmentAttribution INSTANCE;

    private final Map<String, Object> m_values = new HashMap<>();

    MengineFragmentAttribution() {
        super(MengineListenerAttribution.class);

        INSTANCE = this;
    }

    private static native void nativeClearProject();

    public void clearProject() {
        nativeClearProject();
    }

    public void setAttribution(@NonNull String name, @Nullable Object value) {
        MengineUtils.performOnMainThread(() -> {
            synchronized (this) {
                if (m_values.containsKey(name) == true && Objects.equals(m_values.get(name), value) == true) {
                    return;
                }

                m_values.put(name, value);
            }

            this.propagate(MengineListenerAttribution::onMengineAttribution, name, value);
        });
    }
}
