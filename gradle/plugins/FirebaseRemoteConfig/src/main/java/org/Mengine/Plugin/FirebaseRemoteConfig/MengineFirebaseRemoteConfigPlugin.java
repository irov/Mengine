package org.Mengine.Plugin.FirebaseRemoteConfig;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;

import com.google.firebase.remoteconfig.FirebaseRemoteConfig;
import com.google.firebase.remoteconfig.FirebaseRemoteConfigSettings;
import com.google.firebase.remoteconfig.FirebaseRemoteConfigValue;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;

import java.util.HashMap;
import java.util.Map;
import java.util.stream.Collectors;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Bundle;
import androidx.annotation.NonNull;

public class MengineFirebaseRemoteConfigPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "FirebaseRemoteConfig";
    public static final boolean PLUGIN_EMBEDDING = true;

    /**
     * <p>
     * void recordException(Throwable throwable)
     * void recordLog(String msg)
     */

    @Override
    public void onAppCreate(MengineApplication application) {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        FirebaseRemoteConfigSettings configSettings = new FirebaseRemoteConfigSettings.Builder()
                .setMinimumFetchIntervalInSeconds(3600)
                .build();
        remoteConfig.setConfigSettingsAsync(configSettings);

        remoteConfig.setDefaultsAsync(R.xml.remote_config_defaults);
    }

    @Override
    public void onAppTerminate(MengineApplication application) {
    }

    @Override
    public void onAppAttachBaseContext(MengineApplication application, Context base) {
    }

    @Override
    public void onAppConfigurationChanged(MengineApplication application, Configuration newConfig) {
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        remoteConfig.fetchAndActivate()
            .addOnCompleteListener(activity, new OnCompleteListener<Boolean>() {
                @Override
                public void onComplete(@NonNull Task<Boolean> task) {
                    if (task.isSuccessful() == true) {
                        boolean updated = task.getResult();

                        MengineFirebaseRemoteConfigPlugin.this.logMessage("Remote config successful fetch and activate params: %d"
                            , updated
                        );

                        MengineFirebaseRemoteConfigPlugin.this.activateSemaphore("FirebaseRemoteConfigFetchSuccessful");
                    } else {
                        Exception e = task.getException();

                        MengineFirebaseRemoteConfigPlugin.this.logError("Remote config invalid fetch and activate params: %s"
                            , e == null ? "No Exception" : e.getLocalizedMessage()
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
}
