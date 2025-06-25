package org.Mengine.Plugin.FirebaseRemoteConfig;

import android.os.Bundle;

import androidx.annotation.IntegerRes;
import androidx.annotation.NonNull;

import com.google.firebase.remoteconfig.FirebaseRemoteConfig;
import com.google.firebase.remoteconfig.FirebaseRemoteConfigSettings;
import com.google.firebase.remoteconfig.FirebaseRemoteConfigValue;
import com.google.firebase.remoteconfig.internal.DefaultsXmlParser;

import org.Mengine.Base.MengineParamAcquisition;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFragmentRemoteConfig;
import org.Mengine.Base.MengineListenerAcquisition;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class MengineFirebaseRemoteConfigPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity, MengineListenerAcquisition {
    public static final String SERVICE_NAME = "FBRemoteConfig";
    public static final boolean SERVICE_EMBEDDING = true;

    private static final @IntegerRes int METADATA_MINIMUM_FETCH_INTERVAL = R.integer.mengine_firebase_remote_config_minimum_fetch_interval;

    private Map<String, String> m_defaults = new HashMap<>();

    private volatile boolean m_prefetching = false;

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        int MengineFirebaseRemoteConfigPlugin_MinimumFetchInterval = this.getResourceInteger(METADATA_MINIMUM_FETCH_INTERVAL);

        this.logInfo("%s: %d"
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

        remoteConfig.setDefaultsAsync(R.xml.remote_config_defaults)
            .addOnCompleteListener(task -> {
                boolean successful = task.isSuccessful();

                if (successful == false) {
                    Exception e = task.getException();

                    this.logWarning("[WARNING] remote config invalid set defaults params: %s"
                        , e != null ? e.getMessage() : "unknown exception"
                    );
                } else {
                    this.logInfo("remote config defaults set successfully");
                }
            });
    }

    @Override
    public void onAppPost(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        this.fetchRemoteConfigValues(remoteConfig);
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        this.fetchAndActivate(activity);
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
                this.logInfo("remote config invalid key: %s not found in configs: %s"
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
                this.logException(e, Map.of(
                    "key", key,
                    "value", value_string
                ));

                continue;
            }

            configs.put(key, value_json);
        }

        this.logInfo("remote config values: %s"
            , configs
        );

        MengineFragmentRemoteConfig.INSTANCE.remoteConfigFetch(configs);
    }

    protected void fetchAndActivate(@NonNull MengineActivity activity) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("remote config invalid network not available");

            return;
        }

        if (m_prefetching == true) {
            return;
        }

        FirebaseRemoteConfig remoteConfig = FirebaseRemoteConfig.getInstance();

        this.buildEvent("mng_fb_rc_fetch")
            .log();

        m_prefetching = true;

        remoteConfig.fetchAndActivate()
            .addOnSuccessListener(activity, updated -> {
                m_prefetching = false;

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

                this.logInfo("remote config success fetch and activate updated: %b"
                    , updated
                );

                if (updated == true) {
                    this.fetchRemoteConfigValues(remoteConfig);
                }
            }).addOnFailureListener(activity, e -> {
                m_prefetching = false;

                this.buildEvent("mng_fb_rc_fetch_error")
                    .addParameterException("exception", e)
                    .log();

                this.logWarning("[WARNING] remote config invalid fetch and activate params: %s"
                    , e.getMessage()
                );
            }).addOnCanceledListener(activity, () -> {
                m_prefetching = false;

                this.buildEvent("mng_fb_rc_fetch_cancel")
                    .log();

                this.logWarning("[WARNING] remote config invalid fetch and activate canceled");
            });
    }

    @Override
    public void onADIDChange(@NonNull MengineApplication application, @NonNull String adid) {
        //Empty
    }

    @Override
    public void onAcquisitionChange(@NonNull MengineApplication application, @NonNull MengineParamAcquisition acquisition) {
        if (acquisition == null) {
            this.logWarning("remote config invalid activity");

            return;
        }

        MengineActivity activity = this.getMengineActivity();

        this.fetchAndActivate(activity);
    }
}
