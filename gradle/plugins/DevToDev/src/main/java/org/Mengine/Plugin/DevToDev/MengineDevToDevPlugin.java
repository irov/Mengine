package org.Mengine.Plugin.DevToDev;

import android.content.Context;
import androidx.annotation.NonNull;

import com.devtodev.analytics.external.DTDLogLevel;
import com.devtodev.analytics.external.analytics.DTDAnalytics;
import com.devtodev.analytics.external.analytics.DTDAnalyticsConfiguration;
import com.devtodev.analytics.external.analytics.DTDCustomEventParameters;

import org.Mengine.Base.MengineAdRevenueParam;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerAdRevenue;
import org.Mengine.Base.MengineListenerAnalytics;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerGame;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

public class MengineDevToDevPlugin extends MengineService implements MengineListenerAnalytics, MengineListenerAdRevenue, MengineListenerApplication, MengineListenerUser, MengineListenerGame {
    public static final String SERVICE_NAME = "DevToDev";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final String METADATA_APP_ID = "mengine.devtodev.app_id";

    private boolean m_initializeSuccessful;

    public void logEvent(@NonNull String eventName, @NonNull DTDCustomEventParameters params) {
        if (m_initializeSuccessful == false) {
            return;
        }

        DTDAnalytics.INSTANCE.customEvent(eventName, params);
    }

    @Override
    public void onAppPrepare(@NonNull MengineApplication application, @NonNull Map<String, String> pluginVersions) throws MengineServiceInvalidInitializeException {
        m_initializeSuccessful = false;

        String MengineDevToDevPlugin_AppId = this.getMetaDataString(METADATA_APP_ID);

        this.logInfo("%s: %s"
            , METADATA_APP_ID
            , MengineUtils.getRedactedValue(MengineDevToDevPlugin_AppId)
        );

        DTDAnalytics.INSTANCE.setInitializationCompleteCallback(() -> {
            this.logMessage("Initialized DevToDev has been finished");

            m_initializeSuccessful = true;

            return null;
        });

        DTDAnalytics.INSTANCE.coppaControlEnable();

        DTDAnalyticsConfiguration configuration = new DTDAnalyticsConfiguration();

        if (BuildConfig.DEBUG == true) {
            configuration.setLogLevel(DTDLogLevel.Debug);
        } else {
            configuration.setLogLevel(DTDLogLevel.Error);
        }

        String userId = application.getUserId();
        configuration.setUserId(userId);

        Context context = application.getApplicationContext();

        DTDAnalytics.INSTANCE.initialize(MengineDevToDevPlugin_AppId, configuration, context);
    }

    @Override
    public void onMengineChangeUserId(@NonNull MengineApplication application, String userId) {
        if (m_initializeSuccessful == false) {
            return;
        }

        DTDAnalytics.INSTANCE.setUserId(userId);
    }

    @Override
    public void onMengineRemoveUserData(@NonNull MengineApplication application) {
        if (m_initializeSuccessful == false) {
            return;
        }

        DTDAnalytics.INSTANCE.setUserId(null);
    }

    private void updateEventParameters(@NonNull DTDCustomEventParameters dtd, Map<String, Object> parameters) {
        for (Map.Entry<String, Object> entry : parameters.entrySet()) {
            String name = entry.getKey();
            Object parameter = entry.getValue();

            if (parameter instanceof Boolean) {
                dtd.add(name, (Boolean)parameter);
            } else if (parameter instanceof Long) {
                dtd.add(name, (Long)parameter);
            } else if (parameter instanceof Double) {
                dtd.add(name, (Double)parameter);
            } else if (parameter instanceof String) {
                dtd.add(name, (String)parameter);
            } else {
                this.logError("[ERROR] customEvent unsupported parameter: %s class: %s"
                        , parameter
                        , parameter.getClass()
                );
            }
        }
    }

    @Override
    public void onMengineAnalyticsEvent(@NonNull MengineApplication application, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        if (m_initializeSuccessful == false) {
            return;
        }

        DTDCustomEventParameters params = new DTDCustomEventParameters();

        this.updateEventParameters(params, bases);
        this.updateEventParameters(params, parameters);

        DTDAnalytics.INSTANCE.customEvent(eventName, params);
    }

    @Override
    public void onMengineAnalyticsFlush(MengineApplication application) {
        if (m_initializeSuccessful == false) {
            return;
        }

        DTDAnalytics.INSTANCE.sendBufferedEvents();
    }

    @Override
    public void onMengineAdRevenue(MengineApplication application, MengineAdRevenueParam revenue) {
        if (m_initializeSuccessful == false) {
            return;
        }

        String network = revenue.ADREVENUE_NETWORK;
        String adUnitId = revenue.ADREVENUE_ADUNITID;
        String placement = revenue.ADREVENUE_PLACEMENT;
        double revenueValue = revenue.ADREVENUE_REVENUE_VALUE;

        DTDAnalytics.INSTANCE.adImpression(network, revenueValue, placement, adUnitId);
    }

    @Override
    public void onMengineUnlockAchievement(@NonNull MengineApplication application, String achievementId) {
        if (m_initializeSuccessful == false) {
            return;
        }

        //Todo: implement unlock achievement
    }

    @Override
    public void onMengineIncrementAchievement(@NonNull MengineApplication application, String achievementId, int numSteps) {
        if (m_initializeSuccessful == false) {
            return;
        }

        //Todo: implement increment achievement
    }

    @Override
    public void onMengineRevealAchievement(@NonNull MengineApplication application, String achievementId) {
        if (m_initializeSuccessful == false) {
            return;
        }

        //Todo: implement reveal achievement
    }

    @Override
    public void onMengineSubmitLeaderboardScore(@NonNull MengineApplication application, String leaderboardId, long score) {
        if (m_initializeSuccessful == false) {
            return;
        }

        //Todo: implement submit leaderboard score
    }
}
