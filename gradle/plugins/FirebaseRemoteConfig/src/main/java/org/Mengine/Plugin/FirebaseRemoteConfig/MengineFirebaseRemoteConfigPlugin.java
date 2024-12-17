package org.Mengine.Plugin.FirebaseRemoteConfig;

import android.os.Bundle;

import androidx.annotation.NonNull;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.remoteconfig.FirebaseRemoteConfig;
import com.google.firebase.remoteconfig.FirebaseRemoteConfigSettings;
import com.google.firebase.remoteconfig.FirebaseRemoteConfigValue;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.HashMap;
import java.util.Map;
import java.util.stream.Collectors;

public class MengineFirebaseRemoteConfigPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity {
    public static final String SERVICE_NAME = "MengineFBRemoteConfig";
    public static final boolean SERVICE_EMBEDDING = true;

    private static final String METADATA_MINIMUM_FETCH_INTERVAL = "mengine.firebase_remote_config.minimum_fetch_interval";

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        long MengineFirebaseRemoteConfigPlugin_MinimumFetchInterval = this.getMetaDataLong(METADATA_MINIMUM_FETCH_INTERVAL);

        this.logMessage("%s: %d"
            , METADATA_MINIMUM_FETCH_INTERVAL
            , MengineFirebaseRemoteConfigPlugin_MinimumFetchInterval
        );

        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        FirebaseRemoteConfigSettings configSettings = new FirebaseRemoteConfigSettings.Builder()
                .setMinimumFetchIntervalInSeconds(MengineFirebaseRemoteConfigPlugin_MinimumFetchInterval)
                .build();
        remoteConfig.setConfigSettingsAsync(configSettings);

        remoteConfig.setDefaultsAsync(R.xml.remote_config_defaults);
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        remoteConfig.fetchAndActivate()
            .addOnCompleteListener(activity, task -> {
                if (task.isSuccessful() == false) {
                    Exception e = task.getException();

                    this.logError("[ERROR] remote config invalid fetch and activate params: %s"
                        , e == null ? "No Exception" : e.getMessage()
                    );

                    return;
                }

                boolean updated = task.getResult();

                Map<String, FirebaseRemoteConfigValue> allValues = remoteConfig.getAll();

                Map<String, String> allValueString = new HashMap<>();
                for (Map.Entry<String, FirebaseRemoteConfigValue> entry : allValues.entrySet()) {
                    String key = entry.getKey();
                    FirebaseRemoteConfigValue value = entry.getValue();
                    String valueString = value.asString();

                    allValueString.put(key, valueString);
                }

                this.logMessage("remote config successful fetch and activate params: %s [%s]"
                    , allValueString
                    , updated == true ? "updated" : "not updated"
                );

                this.activateSemaphore("FirebaseRemoteConfigFetchSuccessful");

                this.sendEvent(MengineEvent.EVENT_REMOTE_CONFIG_FETCH, updated);
            });
    }

    public Map<String, String> getRemoteConfig() {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        Map<String, FirebaseRemoteConfigValue> remoteValues = remoteConfig.getAll();

        Map<String, String> correctValues;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.N) {
            correctValues = remoteValues.entrySet().stream()
                .collect(Collectors.toMap(Map.Entry::getKey, entry -> entry.getValue().asString()));
        } else {
            correctValues = new HashMap<>();

            for (Map.Entry<String, FirebaseRemoteConfigValue> entry : remoteValues.entrySet()) {
                String key = entry.getKey();
                FirebaseRemoteConfigValue value = entry.getValue();
                String value_str = value.asString();

                correctValues.put(key, value_str);
            }
        }

        return correctValues;
    }

    public String getRemoteConfigValueString(String key) {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        String value = remoteConfig.getString(key);

        return value;
    }

    public boolean getRemoteConfigValueBoolean(String key) {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        boolean value = remoteConfig.getBoolean(key);

        return value;
    }

    public double getRemoteConfigValueDouble(String key) {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        double value = remoteConfig.getDouble(key);

        return value;
    }

    public long getRemoteConfigValueLong(String key) {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        long value = remoteConfig.getLong(key);

        return value;
    }

    public Map<String, Object> getRemoteConfigValueJSON(String key) {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        String value = remoteConfig.getString(key);

        if (value.isEmpty() == true) {
            return new HashMap<>();
        }

        Map<String, Object> mapJson = MengineUtils.parseJSONMap(value);

        return mapJson;
    }
}
