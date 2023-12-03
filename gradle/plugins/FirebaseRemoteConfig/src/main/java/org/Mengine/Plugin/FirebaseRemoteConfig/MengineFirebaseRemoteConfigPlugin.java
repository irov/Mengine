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
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import java.util.HashMap;
import java.util.Map;
import java.util.stream.Collectors;

public class MengineFirebaseRemoteConfigPlugin extends MenginePlugin implements MenginePluginApplicationListener, MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "FirebaseRemoteConfig";
    public static final boolean PLUGIN_EMBEDDING = true;

    /**
     * <p>
     * void recordException(Throwable throwable)
     * void recordLog(String msg)
     */

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        FirebaseRemoteConfigSettings configSettings = new FirebaseRemoteConfigSettings.Builder()
                .setMinimumFetchIntervalInSeconds(3600)
                .build();
        remoteConfig.setConfigSettingsAsync(configSettings);

        remoteConfig.setDefaultsAsync(R.xml.remote_config_defaults);
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        remoteConfig.fetchAndActivate()
            .addOnCompleteListener(activity, new OnCompleteListener<Boolean>() {
                @Override
                public void onComplete(@NonNull Task<Boolean> task) {
                    if (task.isSuccessful() == true) {
                        boolean updated = task.getResult();

                        MengineFirebaseRemoteConfigPlugin.this.logMessage("remote config successful fetch and activate params: %s [%s]"
                            , remoteConfig.getAll()
                            , updated == true ? "updated" : "not updated"
                        );

                        MengineFirebaseRemoteConfigPlugin.this.activateSemaphore("FirebaseRemoteConfigFetchSuccessful");

                        MengineFirebaseRemoteConfigPlugin.this.sendEvent(MengineEvent.EVENT_REMOTE_CONFIG_FETCH, updated);
                    } else {
                        Exception e = task.getException();

                        MengineFirebaseRemoteConfigPlugin.this.logError("[ERROR] remote config invalid fetch and activate params: %s"
                            , e == null ? "No Exception" : e.getMessage()
                        );
                    }
                }
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
}
