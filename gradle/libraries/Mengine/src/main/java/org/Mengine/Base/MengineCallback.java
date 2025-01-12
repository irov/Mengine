package org.Mengine.Base;

import java.util.Map;

@FunctionalInterface
public interface MengineCallback {
    void call(boolean successful, Map<String, Object> params);
}