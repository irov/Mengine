package org.Mengine.Plugin.FirebaseAnalytics;

import android.os.Bundle;

import androidx.annotation.NonNull;

import com.google.firebase.analytics.FirebaseAnalytics;

import org.Mengine.Base.MengineParamAcquisition;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineParamAdRevenue;
import org.Mengine.Base.MengineParamAnalyticsEvent;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerAcquisition;
import org.Mengine.Base.MengineListenerGame;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerAdRevenue;
import org.Mengine.Base.MengineListenerAnalytics;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineUtils;

import java.util.EnumMap;
import java.util.Map;

public class MengineFirebaseAnalyticsPlugin extends MengineService implements MengineListenerAnalytics, MengineListenerAdRevenue, MengineListenerTransparencyConsent, MengineListenerApplication, MengineListenerUser, MengineListenerGame, MengineListenerAcquisition {
    public static final String SERVICE_NAME = "FBAnalytics";
    public static final boolean SERVICE_EMBEDDING = true;

    FirebaseAnalytics m_firebaseAnalytics;

    @Override
    public void onAppPrepare(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        FirebaseAnalytics firebaseAnalytics = FirebaseAnalytics.getInstance(application);

        String userId = application.getUserId();
        if (userId != null) {
            firebaseAnalytics.setUserId(userId);
        }

        String installId = application.getInstallId();
        long installTimestamp = application.getInstallTimestamp();
        String installVersion = application.getInstallVersion();
        long installRND = application.getInstallRND();
        long sessionIndex = application.getSessionIndex();
        long sessionTimestamp = application.getSessionTimestamp();

        boolean is_publish = application.isBuildPublish();

        firebaseAnalytics.setUserProperty("is_publish", String.valueOf(is_publish));
        firebaseAnalytics.setUserProperty("is_debug", String.valueOf(BuildConfig.DEBUG));
        firebaseAnalytics.setUserProperty("install_id", installId);
        firebaseAnalytics.setUserProperty("install_timestamp", String.valueOf(installTimestamp));
        firebaseAnalytics.setUserProperty("install_version", installVersion);
        firebaseAnalytics.setUserProperty("install_rnd", String.valueOf(installRND));
        firebaseAnalytics.setUserProperty("session_index", String.valueOf(sessionIndex));
        firebaseAnalytics.setUserProperty("session_timestamp", String.valueOf(sessionTimestamp));

        String acquisitionNetwork = application.getAcquisitionNetwork();
        String acquisitionCampaign = application.getAcquisitionCampaign();

        firebaseAnalytics.setUserProperty("acquisition_network", acquisitionNetwork);
        firebaseAnalytics.setUserProperty("acquisition_campaign", acquisitionCampaign);

        long currentTimestamp = MengineUtils.getTimestamp();
        long lifeTime = currentTimestamp - installTimestamp;

        firebaseAnalytics.setUserProperty("life_time", String.valueOf(lifeTime));

        m_firebaseAnalytics = firebaseAnalytics;
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        m_firebaseAnalytics = null;
    }

    @Override
    public void onMengineChangeUserId(@NonNull MengineApplication application, String oldUserId, String newUserId) {
        m_firebaseAnalytics.setUserId(newUserId);
    }

    @Override
    public void onMengineRemoveUserData(@NonNull MengineApplication application) {
        m_firebaseAnalytics.resetAnalyticsData();
        m_firebaseAnalytics.setUserId(null);
    }

    private void updateBundle(@NonNull Bundle bundle, @NonNull Map<String, Object> parameters) {
        for (Map.Entry<String, Object> entry : parameters.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();

            if (value == null) {
                bundle.putString(key, null);
            } else if (value instanceof Boolean boolValue) {
                bundle.putBoolean(key, boolValue);
            } else if (value instanceof Integer intValue) {
                bundle.putInt(key, intValue);
            } else if (value instanceof Long longValue) {
                bundle.putLong(key, longValue);
            } else if (value instanceof Double doubleValue) {
                bundle.putDouble(key, doubleValue);
            } else if (value instanceof String stringValue) {
                if (stringValue.length() > 100) {
                    String stringValue100 = stringValue.substring(0, 100);

                    bundle.putString(key, stringValue100);
                } else {
                    bundle.putString(key, stringValue);
                }
            } else {
                this.logError("[ERROR] unsupported parameter: %s value: %s class: %s"
                    , key
                    , value
                    , value.getClass()
                );
            }
        }
    }

    @Override
    public void onMengineAnalyticsEvent(@NonNull MengineApplication application, @NonNull MengineParamAnalyticsEvent param) {
        Bundle bundle = new Bundle();

        this.updateBundle(bundle, param.ANALYTICS_BASES);
        this.updateBundle(bundle, param.ANALYTICS_PARAMETERS);

        m_firebaseAnalytics.logEvent(param.ANALYTICS_NAME, bundle);
    }

    @Override
    public void onMengineAnalyticsScreenView(@NonNull MengineApplication application, @NonNull String screenType, @NonNull String screenName) {
        Bundle params = new Bundle();

        params.putString(FirebaseAnalytics.Param.SCREEN_CLASS, screenType);
        params.putString(FirebaseAnalytics.Param.SCREEN_NAME, screenName);

        m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.SCREEN_VIEW, params);
    }

    private static String getAdFormat(MengineAdFormat adType) {
        if (adType == MengineAdFormat.ADFORMAT_BANNER) {
            return "BANNER";
        } else if (adType == MengineAdFormat.ADFORMAT_MREC) {
            return "MREC";
        } else if (adType == MengineAdFormat.ADFORMAT_LEADER) {
            return "LEADER";
        } else if (adType == MengineAdFormat.ADFORMAT_INTERSTITIAL) {
            return "INTERSTITIAL";
        } else if (adType == MengineAdFormat.ADFORMAT_APPOPEN) {
            return "APPOPEN";
        } else if (adType == MengineAdFormat.ADFORMAT_REWARDED) {
            return "REWARDED";
        } else if (adType == MengineAdFormat.ADFORMAT_NATIVE) {
            return "NATIVE";
        }

        return "UNKNOWN";
    }

    private static String getAdMediation(MengineAdMediation adMediation) {
        if (adMediation == MengineAdMediation.ADMEDIATION_APPLOVINMAX) {
            return "appLovin";
        }

        if (adMediation == MengineAdMediation.ADMEDIATION_ADMOB) {
            return "adMob";
        }

        return "unknown";
    }

    @Override
    public void onMengineAdRevenue(@NonNull MengineApplication application, @NonNull MengineParamAdRevenue revenue) {
        MengineAdMediation adMediation = revenue.ADREVENUE_MEDIATION;
        String adFirebaseMediation = MengineFirebaseAnalyticsPlugin.getAdMediation(adMediation);
        String networkName = revenue.ADREVENUE_NETWORK;
        MengineAdFormat adFormat = revenue.ADREVENUE_FORMAT;
        String format = MengineFirebaseAnalyticsPlugin.getAdFormat(adFormat);
        String adUnitId = revenue.ADREVENUE_ADUNITID;
        double revenueValue = revenue.ADREVENUE_REVENUE_VALUE;
        String revenueCurrency = revenue.ADREVENUE_REVENUE_CURRENCY;

        Bundle params = new Bundle();
        params.putString(FirebaseAnalytics.Param.AD_PLATFORM, adFirebaseMediation);
        params.putString(FirebaseAnalytics.Param.AD_SOURCE, networkName);
        params.putString(FirebaseAnalytics.Param.AD_FORMAT, format);
        params.putString(FirebaseAnalytics.Param.AD_UNIT_NAME, adUnitId);
        params.putDouble(FirebaseAnalytics.Param.VALUE, revenueValue);
        params.putString(FirebaseAnalytics.Param.CURRENCY, revenueCurrency);

        m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.AD_IMPRESSION, params);
    }

    @Override
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, @NonNull MengineParamTransparencyConsent tcParam) {
        boolean AD_STORAGE = tcParam.getConsentAdStorage();
        boolean ANALYTICS_STORAGE =  tcParam.getConsentAnalyticsStorage();
        boolean AD_USER_DATA = tcParam.getConsentAdUserData();
        boolean AD_PERSONALIZATION = tcParam.getConsentAdPersonalization();

        Map<FirebaseAnalytics.ConsentType, FirebaseAnalytics.ConsentStatus> consentMap = new EnumMap<>(FirebaseAnalytics.ConsentType.class);
        consentMap.put(FirebaseAnalytics.ConsentType.AD_STORAGE, AD_STORAGE ? FirebaseAnalytics.ConsentStatus.GRANTED : FirebaseAnalytics.ConsentStatus.DENIED);
        consentMap.put(FirebaseAnalytics.ConsentType.ANALYTICS_STORAGE, ANALYTICS_STORAGE ? FirebaseAnalytics.ConsentStatus.GRANTED : FirebaseAnalytics.ConsentStatus.DENIED);
        consentMap.put(FirebaseAnalytics.ConsentType.AD_USER_DATA, AD_USER_DATA ? FirebaseAnalytics.ConsentStatus.GRANTED : FirebaseAnalytics.ConsentStatus.DENIED);
        consentMap.put(FirebaseAnalytics.ConsentType.AD_PERSONALIZATION, AD_PERSONALIZATION ? FirebaseAnalytics.ConsentStatus.GRANTED : FirebaseAnalytics.ConsentStatus.DENIED);

        this.logInfo("setConsent AD_STORAGE: %s ANALYTICS_STORAGE: %s AD_USER_DATA: %s AD_PERSONALIZATION: %s"
            , AD_STORAGE
            , ANALYTICS_STORAGE
            , AD_USER_DATA
            , AD_PERSONALIZATION
        );

        m_firebaseAnalytics.setConsent(consentMap);
    }

    @Override
    public void onMengineUnlockAchievement(@NonNull MengineApplication application, String achievementId) {
        Bundle params = new Bundle();
        params.putString(FirebaseAnalytics.Param.ACHIEVEMENT_ID, achievementId);

        m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.UNLOCK_ACHIEVEMENT, params);
    }

    @Override
    public void onMengineIncrementAchievement(@NonNull MengineApplication application, String achievementId, int numSteps) {
        //Todo: implement increment achievement
    }

    @Override
    public void onMengineRevealAchievement(@NonNull MengineApplication application, String achievementId) {
        //Todo: implement reveal achievement
    }

    @Override
    public void onMengineSubmitLeaderboardScore(@NonNull MengineApplication application, String leaderboardId, long score) {
        Bundle params = new Bundle();
        params.putLong(FirebaseAnalytics.Param.SCORE, score);
        params.putString(FirebaseAnalytics.Param.CHARACTER, "LEADERBOARD_" + leaderboardId);

        m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.POST_SCORE, params);
    }

    @Override
    public void onADIDChange(@NonNull MengineApplication application, @NonNull String adid) {
        //Empty
    }

    @Override
    public void onAcquisitionChange(@NonNull MengineApplication application, @NonNull MengineParamAcquisition acquisition) {
        m_firebaseAnalytics.setUserProperty("acquisition_network", acquisition.ACQUISITION_NETWORK);
        m_firebaseAnalytics.setUserProperty("acquisition_campaign", acquisition.ACQUISITION_CAMPAIGN);
    }
}
