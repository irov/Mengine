package org.Mengine.Base;

import java.util.Map;

@FunctionalInterface
public interface MengineCallback {
    void call(boolean successful, Map<String, Object> params);

    static MengineCallback onMainThread(MengineCallback callback) {
        return new MengineCallback() {
            @Override
            public void call(boolean successful, Map<String, Object> params) {
                MengineUtils.performOnMainThread(() -> {
                    callback.call(successful, params);
                });
            }
        };
    }
}
