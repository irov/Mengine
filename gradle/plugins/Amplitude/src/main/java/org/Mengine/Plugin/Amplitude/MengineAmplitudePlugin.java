package org.Mengine.Plugin.Amplitude;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

import com.amplitude.android.Amplitude;
import com.amplitude.android.AutocaptureOption;
import com.amplitude.android.Configuration;
import com.amplitude.core.events.Revenue;
import com.amplitude.core.network.NetworkTrackingPlugin;
import com.amplitude.core.events.Identify;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerAdRevenue;
import org.Mengine.Base.MengineListenerAnalytics;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineParamAdRevenue;
import org.Mengine.Base.MengineParamAnalyticsEvent;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.HashMap;
import java.util.Map;

import okhttp3.OkHttpClient;

public class MengineAmplitudePlugin extends MengineService implements MengineListenerApplication, MengineListenerUser, MengineListenerAnalytics, MengineListenerAdRevenue {
    public static final String SERVICE_NAME = "Amplitude";
    public static final boolean SERVICE_EMBEDDING = true;
    public static final int SAVE_VERSION = 1;

    protected static final @StringRes int METADATA_API_KEY = R.string.mengine_amplitude_api_key;

    protected Amplitude m_amplitude;

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        String MengineAmplitudePlugin_ApiKey = this.getResourceString(METADATA_API_KEY);

        Configuration configuration = new Configuration(MengineAmplitudePlugin_ApiKey, application);
        configuration.getAutocapture().add(AutocaptureOption.SESSIONS);
        configuration.getAutocapture().add(AutocaptureOption.APP_LIFECYCLES);
        configuration.getAutocapture().add(AutocaptureOption.SCREEN_VIEWS);
        configuration.setFlushEventsOnClose(true);

        Amplitude amplitude = new Amplitude(configuration);

        Identify identify = new Identify();

        String installKey = application.getInstallKey();
        long installTimestamp = application.getInstallTimestamp();
        String installVersion = application.getInstallVersion();
        long installRND = application.getInstallRND();
        long sessionIndex = application.getSessionIndex();
        long sessionTimestamp = application.getSessionTimestamp();

        identify.set("is_dev", String.valueOf(BuildConfig.DEBUG));
        identify.set("install_timestamp", String.valueOf(installTimestamp));
        identify.set("install_version", installVersion);
        identify.set("install_rnd", String.valueOf(installRND));
        identify.set("session_index", String.valueOf(sessionIndex));
        identify.set("session_timestamp", String.valueOf(sessionTimestamp));

        String acquisitionNetwork = application.getAcquisitionNetwork();
        String acquisitionCampaign = application.getAcquisitionCampaign();

        identify.set("acquisition_network", acquisitionNetwork);
        identify.set("acquisition_campaign", acquisitionCampaign);

        long currentTimestamp = MengineUtils.getTimestamp();
        long lifeTime = currentTimestamp - installTimestamp;

        identify.set("life_time", String.valueOf(lifeTime));

        amplitude.identify(identify);

        String userId = application.getUserId();

        if (userId != null) {
            amplitude.setUserId(userId);
        }

        amplitude.setDeviceId(installKey);

        NetworkTrackingPlugin networkTrackingPlugin = new NetworkTrackingPlugin();

        OkHttpClient okHttpClient = new OkHttpClient.Builder()
            .addInterceptor(networkTrackingPlugin)
            .build();

        amplitude.add(networkTrackingPlugin);

        m_amplitude = amplitude;
    }

    @Override
    public void onMengineChangeUserId(@NonNull MengineApplication application, String oldUserId, String newUserId) {
        if (m_amplitude == null) {
            return;
        }

        m_amplitude.setUserId(newUserId);
    }

    @Override
    public void onMengineRemoveUserData(@NonNull MengineApplication application) {
        if (m_amplitude == null) {
            return;
        }

        m_amplitude.reset();
    }

    @Override
    public void onMengineAnalyticsEvent(@NonNull MengineApplication application, @NonNull MengineParamAnalyticsEvent param) {
        if (m_amplitude == null) {
            return;
        }

        Map<String, Object> total = new HashMap<>();
        total.putAll(param.ANALYTICS_BASES);
        total.putAll(param.ANALYTICS_PARAMETERS);

        m_amplitude.track(param.ANALYTICS_NAME, total);
    }

    @Override
    public void onMengineAnalyticsScreenView(@NonNull MengineApplication application, @NonNull String screenType, @NonNull String screenName) {
        if (m_amplitude == null) {
            return;
        }

        Map<String, Object> eventProperties = new HashMap<>();
        eventProperties.put("screen_type", screenType);
        eventProperties.put("screen_name", screenName);

        m_amplitude.track("screen_view", eventProperties);
    }

    @Override
    public void onMengineAnalyticsFlush(@NonNull MengineApplication application) {
        if (m_amplitude == null) {
            return;
        }

        m_amplitude.flush();
    }

    @Override
    public void onMengineAdRevenue(@NonNull MengineApplication application, @NonNull MengineParamAdRevenue revenue) {
        if (m_amplitude == null) {
            return;
        }

        Revenue r = new Revenue();
        r.setCurrency(revenue.ADREVENUE_REVENUE_CURRENCY);
        r.setPrice(revenue.ADREVENUE_REVENUE_VALUE);
        r.setRevenueType(revenue.ADREVENUE_FORMAT.getName());

        Map<String, Object> properties = new HashMap<>();
        properties.put("ad_mediation", revenue.ADREVENUE_MEDIATION.getName());
        properties.put("ad_unit_id", revenue.ADREVENUE_ADUNITID);
        properties.put("placement", revenue.ADREVENUE_PLACEMENT);
        properties.put("network", revenue.ADREVENUE_NETWORK);

        r.setProperties(properties);

        m_amplitude.revenue(r);
    }
}