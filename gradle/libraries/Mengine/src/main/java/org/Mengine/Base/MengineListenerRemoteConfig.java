package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

import java.util.Map;

public interface MengineListenerRemoteConfig extends MengineServiceInterface {
    void onMengineRemoteConfigFetch(@NonNull MengineApplication application, @NonNull Map<String, JSONObject> configs);
}