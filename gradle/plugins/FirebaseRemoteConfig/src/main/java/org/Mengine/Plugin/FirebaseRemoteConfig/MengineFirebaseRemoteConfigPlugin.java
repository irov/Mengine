package org.Mengine.Plugin.FirebaseRemoteConfig;

import android.os.Bundle;

import androidx.annotation.NonNull;

import com.google.firebase.remoteconfig.FirebaseRemoteConfig;
import com.google.firebase.remoteconfig.FirebaseRemoteConfigSettings;
import com.google.firebase.remoteconfig.FirebaseRemoteConfigValue;
import com.google.firebase.remoteconfig.internal.DefaultsXmlParser;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class MengineFirebaseRemoteConfigPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity {
    public static final String SERVICE_NAME = "FBRemoteConfig";
    public static final boolean SERVICE_EMBEDDING = true;

    private static final String METADATA_MINIMUM_FETCH_INTERVAL = "mengine.firebase_remote_config.minimum_fetch_interval";

    private Map<String, JSONObject> m_configs = new HashMap<>();
    private Map<String, String> m_defaults = new HashMap<>();

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        long MengineFirebaseRemoteConfigPlugin_MinimumFetchInterval = this.getMetaDataLong(METADATA_MINIMUM_FETCH_INTERVAL);

        this.logMessage("%s: %d"
            , METADATA_MINIMUM_FETCH_INTERVAL
            , MengineFirebaseRemoteConfigPlugin_MinimumFetchInterval
        );

        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        FirebaseRemoteConfigSettings.Builder configSettingsBuilder = new FirebaseRemoteConfigSettings.Builder();

        if (BuildConfig.DEBUG == true) {
            configSettingsBuilder.setMinimumFetchIntervalInSeconds(0);
        } else {
            configSettingsBuilder.setMinimumFetchIntervalInSeconds(MengineFirebaseRemoteConfigPlugin_MinimumFetchInterval);
        }

        FirebaseRemoteConfigSettings configSettings = configSettingsBuilder.build();
        remoteConfig.setConfigSettingsAsync(configSettings);

        Map<String, String> defaults = DefaultsXmlParser.getDefaultsFromXml(application, R.xml.remote_config_defaults);
        this.setRemoteConfigDefaults(defaults);

        this.buildEvent("mng_fb_rc_defaults")
            .log();

        remoteConfig.setDefaultsAsync(R.xml.remote_config_defaults)
            .addOnSuccessListener(aVoid -> {
                this.buildEvent("mng_fb_rc_defaults_success")
                    .log();

                this.fetchRemoteConfigValues(remoteConfig);
            })
            .addOnFailureListener(e -> {
                this.buildEvent("mng_fb_rc_defaults_error")
                    .addParameterException("exception", e)
                    .log();

                this.fetchRemoteConfigValues(remoteConfig);
            });
    }

    @Override
    public void onAppPost(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        this.fetchRemoteConfigValues(remoteConfig);
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("remote config invalid network not available");

            return;
        }

        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        this.buildEvent("mng_fb_rc_fetch")
            .log();

        remoteConfig.fetchAndActivate()
            .addOnSuccessListener(activity, updated -> {
                this.buildEvent("mng_fb_rc_fetch_success")
                    .addParameterBoolean("updated", updated)
                    .log();

                Map<String, FirebaseRemoteConfigValue> allValues = remoteConfig.getAll();

                Map<String, String> allValueString = new HashMap<>();
                for (Map.Entry<String, FirebaseRemoteConfigValue> entry : allValues.entrySet()) {
                    String key = entry.getKey();
                    FirebaseRemoteConfigValue value = entry.getValue();
                    String valueString = value.asString();

                    allValueString.put(key, valueString);
                }

                this.logMessage("remote config success fetch and activate updated: %b"
                    , updated
                );

                if (updated == true) {
                    this.fetchRemoteConfigValues(remoteConfig);
                }
            })
            .addOnFailureListener(activity, e -> {
                this.buildEvent("mng_fb_rc_fetch_error")
                    .addParameterException("exception", e)
                    .log();

                this.logError("[ERROR] remote config invalid fetch and activate params: %s"
                    , e.getMessage()
                );
            });
    }

    protected void setRemoteConfigDefaults(Map<String, String> defaults) {
        m_defaults = defaults;

        if (BuildConfig.DEBUG == true) {
            for (Map.Entry<String, String> entry : defaults.entrySet()) {
                String key = entry.getKey();
                String value = entry.getValue();

                try {
                    new JSONObject(value);
                } catch (final JSONException e) {
                    this.assertionError("remote config default invalid json key: %s value: %s exception: %s"
                        , key
                        , value
                        , e.getMessage()
                    );
                }
            }
        }
    }

    protected void fetchRemoteConfigValues(FirebaseRemoteConfig remoteConfig) {
        Map<String, FirebaseRemoteConfigValue> remoteValues = remoteConfig.getAll();

        Map<String, JSONObject> configs = new HashMap<>();

        for (Map.Entry<String, String> entry : m_defaults.entrySet()) {
            String key = entry.getKey();

            FirebaseRemoteConfigValue value = remoteValues.get(key);

            String value_string;

            if (value == null) {
                this.logError("remote config invalid key: %s not found in configs: %s"
                    , key
                    , remoteValues.keySet()
                );

                value_string = m_defaults.get(key);
            } else {
                value_string = value.asString();
            }

            JSONObject value_json;

            try {
                value_json = new JSONObject(value_string);
            } catch (final JSONException e) {
                this.logError("remote config invalid json key: %s value: %s exception: %s"
                    , key
                    , value_string
                    , e.getMessage()
                );

                continue;
            }

            configs.put(key, value_json);
        }

        this.logMessage("remote config values: %s"
            , configs
        );

        synchronized (this) {
            this.m_configs = configs;
        }

        MengineApplication application = this.getMengineApplication();

        application.onMengineRemoteConfigFetch(configs);
    }

    public Map<String, JSONObject> getRemoteConfigs() {
        synchronized (this) {
            return this.m_configs;
        }
    }

    public JSONObject getRemoteConfigValue(String key) {
        synchronized (this) {
            JSONObject value = this.m_configs.get(key);

            return value;
        }
    }
}
