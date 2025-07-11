package org.Mengine.Plugin.DevToDev;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

import com.devtodev.analytics.external.DTDLogLevel;
import com.devtodev.analytics.external.analytics.DTDAnalytics;
import com.devtodev.analytics.external.analytics.DTDAnalyticsConfiguration;
import com.devtodev.analytics.external.analytics.DTDCustomEventParameters;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineParamAdRevenue;
import org.Mengine.Base.MengineAnalyticsEventCategory;
import org.Mengine.Base.MengineParamAnalyticsEvent;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerAdRevenue;
import org.Mengine.Base.MengineListenerAnalytics;
import org.Mengine.Base.MengineListenerGame;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

public class MengineDevToDevPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity, MengineListenerAnalytics, MengineListenerAdRevenue, MengineListenerUser, MengineListenerTransparencyConsent, MengineListenerGame {
    public static final String SERVICE_NAME = "DevToDev";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final @StringRes int METADATA_APP_ID = R.string.mengine_devtodev_app_id;

    private boolean m_initializeSuccessful;

    public void logEvent(@NonNull String eventName, @NonNull DTDCustomEventParameters params) {
        if (m_initializeSuccessful == false) {
            return;
        }

        DTDAnalytics.INSTANCE.customEvent(eventName, params);
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        m_initializeSuccessful = false;

        String MengineDevToDevPlugin_AppId = this.getResourceString(METADATA_APP_ID);

        this.logInfo("%s: %s"
            , METADATA_APP_ID
            , MengineUtils.getRedactedValue(MengineDevToDevPlugin_AppId)
        );

        DTDAnalytics.INSTANCE.setInitializationCompleteCallback(() -> {
            this.logInfo("Initialized DevToDev has been finished");

            m_initializeSuccessful = true;

            return null;
        });

        DTDAnalytics.INSTANCE.coppaControlEnable();

        DTDAnalyticsConfiguration configuration = new DTDAnalyticsConfiguration();

        if (BuildConfig.MENGINE_APP_PLUGIN_DEVTODEV_DEBUG_LOG == true) {
            configuration.setLogLevel(DTDLogLevel.Debug);
        } else if (BuildConfig.DEBUG == true) {
            configuration.setLogLevel(DTDLogLevel.Warning);
        } else {
            configuration.setLogLevel(DTDLogLevel.Error);
        }

        String userId = this.getUserId();
        if (userId != null) {
            configuration.setUserId(userId);
        }

        DTDAnalytics.INSTANCE.initialize(MengineDevToDevPlugin_AppId, configuration, application);
    }

    @Override
    public void onMengineChangeUserId(@NonNull MengineApplication application, String oldUserId, String newUserId) {
        if (m_initializeSuccessful == false) {
            return;
        }

        if (oldUserId == null) {
            DTDAnalytics.INSTANCE.setUserId(newUserId);
        } else {
            DTDAnalytics.INSTANCE.replaceUserId(oldUserId, newUserId);
        }
    }

    @Override
    public void onMengineRemoveUserData(@NonNull MengineApplication application) {
        if (m_initializeSuccessful == false) {
            return;
        }

        //ToDo
    }

    @Override
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, @NonNull MengineParamTransparencyConsent tcParam) {
        if (m_initializeSuccessful == false) {
            return;
        }

        boolean trackingAvailability = true;

        if (tcParam.isPending() == true) {
            trackingAvailability = false;
        } else if (tcParam.getConsentAnalyticsStorage() == false) {
            trackingAvailability = false;
        }

        DTDAnalytics.INSTANCE.setTrackingAvailability(trackingAvailability);
    }

    private void updateEventParameters(@NonNull DTDCustomEventParameters dtd, Map<String, Object> parameters) {
        for (Map.Entry<String, Object> entry : parameters.entrySet()) {
            String name = entry.getKey();
            Object parameter = entry.getValue();

            if (parameter == null) {
                dtd.add(name, "null");
            } else if (parameter instanceof Boolean) {
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
    public void onMengineAnalyticsEvent(@NonNull MengineApplication application, @NonNull MengineParamAnalyticsEvent param) {
        if (m_initializeSuccessful == false) {
            return;
        }

        if (param.ANALYTICS_CATEGORY == MengineAnalyticsEventCategory.MengineAnalyticsEventCategory_System) {
            return;
        }

        DTDCustomEventParameters params = new DTDCustomEventParameters();

        this.updateEventParameters(params, param.ANALYTICS_BASES);
        this.updateEventParameters(params, param.ANALYTICS_PARAMETERS);

        DTDAnalytics.INSTANCE.customEvent(param.ANALYTICS_NAME, params);
    }

    @Override
    public void onMengineAnalyticsFlush(@NonNull MengineApplication application) {
        if (m_initializeSuccessful == false) {
            return;
        }

        DTDAnalytics.INSTANCE.sendBufferedEvents();
    }

    @Override
    public void onMengineAdRevenue(@NonNull MengineApplication application, @NonNull MengineParamAdRevenue revenue) {
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
