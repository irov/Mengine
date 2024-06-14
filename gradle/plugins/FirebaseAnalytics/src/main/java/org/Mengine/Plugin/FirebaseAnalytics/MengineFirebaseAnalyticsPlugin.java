package org.Mengine.Plugin.FirebaseAnalytics;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.Size;

import com.google.firebase.analytics.FirebaseAnalytics;

import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdRevenueParam;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MengineInAppPurchaseParam;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginAdRevenueListener;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MengineTransparencyConsentParam;

import java.util.EnumMap;
import java.util.Map;

public class MengineFirebaseAnalyticsPlugin extends MenginePlugin implements MenginePluginAnalyticsListener, MenginePluginAdRevenueListener, MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "MengineFBAnalytics";
    public static final boolean PLUGIN_EMBEDDING = true;

    FirebaseAnalytics m_firebaseAnalytics;

    @Override
    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_SESSION_ID) {
            String sessionId = (String)args[0];

            m_firebaseAnalytics.setUserId(sessionId);
        } else if (event == MengineEvent.EVENT_TRANSPARENCY_CONSENT) {
            MengineTransparencyConsentParam consent = (MengineTransparencyConsentParam)args[0];

            boolean AD_STORAGE = consent.getConsentAdStorage();
            boolean ANALYTICS_STORAGE =  consent.getConsentAnalyticsStorage();
            boolean AD_USER_DATA = consent.getConsentAdUserData();
            boolean AD_PERSONALIZATION = consent.getConsentAdPersonalization();

            Map<FirebaseAnalytics.ConsentType, FirebaseAnalytics.ConsentStatus> consentMap = new EnumMap<>(FirebaseAnalytics.ConsentType.class);
            consentMap.put(FirebaseAnalytics.ConsentType.ANALYTICS_STORAGE, ANALYTICS_STORAGE ? FirebaseAnalytics.ConsentStatus.GRANTED : FirebaseAnalytics.ConsentStatus.DENIED);
            consentMap.put(FirebaseAnalytics.ConsentType.AD_STORAGE, AD_STORAGE ? FirebaseAnalytics.ConsentStatus.GRANTED : FirebaseAnalytics.ConsentStatus.DENIED);
            consentMap.put(FirebaseAnalytics.ConsentType.AD_USER_DATA, AD_USER_DATA ? FirebaseAnalytics.ConsentStatus.GRANTED : FirebaseAnalytics.ConsentStatus.DENIED);
            consentMap.put(FirebaseAnalytics.ConsentType.AD_PERSONALIZATION, AD_PERSONALIZATION ? FirebaseAnalytics.ConsentStatus.GRANTED : FirebaseAnalytics.ConsentStatus.DENIED);

            this.logMessage("setConsent AD_STORAGE: %s ANALYTICS_STORAGE: %s AD_USER_DATA: %s AD_PERSONALIZATION: %s"
                , AD_STORAGE
                , ANALYTICS_STORAGE
                , AD_USER_DATA
                , AD_PERSONALIZATION
            );

            m_firebaseAnalytics.setConsent(consentMap);
        }
    }

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        FirebaseAnalytics firebaseAnalytics = FirebaseAnalytics.getInstance(application);

        String sessionId = application.getSessionId();
        firebaseAnalytics.setUserId(sessionId);

        String installKey = application.getInstallKey();
        long installTimestamp = application.getInstallTimestamp();
        String installVersion = application.getInstallVersion();
        long installRND = application.getInstallRND();
        long sessionIndex = application.getSessionIndex();
        long sessionTimestamp = application.getSessionTimestamp();
        long purchasesTimestamp = application.getPurchasesTimestamp();

        firebaseAnalytics.setUserProperty("is_dev", String.valueOf(BuildConfig.DEBUG));
        firebaseAnalytics.setUserProperty("install_key", installKey);
        firebaseAnalytics.setUserProperty("install_timestamp", String.valueOf(installTimestamp));
        firebaseAnalytics.setUserProperty("install_version", installVersion);
        firebaseAnalytics.setUserProperty("install_rnd", String.valueOf(installRND));
        firebaseAnalytics.setUserProperty("session_index", String.valueOf(sessionIndex));
        firebaseAnalytics.setUserProperty("session_timestamp", String.valueOf(sessionTimestamp));
        firebaseAnalytics.setUserProperty("purchases_timestamp", String.valueOf(purchasesTimestamp));

        long currentTime = System.currentTimeMillis();
        long lifeTime = currentTime - installTimestamp;

        firebaseAnalytics.setUserProperty("life_time", String.valueOf(lifeTime));

        m_firebaseAnalytics = firebaseAnalytics;
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        if (m_firebaseAnalytics == null) {
            return;
        }

        Bundle params = new Bundle();

        for (Map.Entry<String, Object> entry : bases.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();

            if (value instanceof Boolean) {
                params.putBoolean(key, (Boolean)value);
            } else if (value instanceof Long) {
                params.putLong(key, (Long)value);
            } else if (value instanceof Double) {
                params.putDouble(key, (Double)value);
            } else if (value instanceof String) {
                String stringValue = (String)value;

                if (stringValue.length() > 100) {
                    String stringValue100 = stringValue.substring(0, 100);

                    params.putString(key, stringValue100);
                } else {
                    params.putString(key, stringValue);
                }
            } else {
                this.logError("[ERROR] unsupported parameter: %s class: %s"
                    , value
                    , value == null ? "null" : value.getClass()
                );

                return;
            }
        }

        for (Map.Entry<String, Object> entry : parameters.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();

            if (value instanceof Boolean) {
                params.putBoolean(key, (Boolean)value);
            } else if (value instanceof Long) {
                params.putLong(key, (Long)value);
            } else if (value instanceof Double) {
                params.putDouble(key, (Double)value);
            } else if (value instanceof String) {
                String stringValue = (String)value;

                if (stringValue.length() > 100) {
                    String stringValue100 = stringValue.substring(0, 100);

                    params.putString(key, stringValue100);
                } else {
                    params.putString(key, stringValue);
                }
            } else {
                this.logError("[ERROR] unsupported parameter: %s class: %s"
                    , value
                    , value == null ? "null" : value.getClass()
                );

                return;
            }
        }

        m_firebaseAnalytics.logEvent(eventName, params);
    }

    @Override
    public void onMengineAnalyticsScreenView(MengineApplication application, String screenType, String screenName) {
        if (m_firebaseAnalytics == null) {
            return;
        }

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
        } else if (adType == MengineAdFormat.ADFORMAT_APP_OPEN) {
            return "APP_OPEN";
        } else if (adType == MengineAdFormat.ADFORMAT_REWARDED) {
            return "REWARDED";
        } else if (adType == MengineAdFormat.ADFORMAT_REWARDED_INTERSTITIAL) {
            return "REWARDED_INTERSTITIAL";
        } else if (adType == MengineAdFormat.ADFORMAT_NATIVE) {
            return "NATIVE";
        }

        return "UNKNOWN";
    }

    private static String getAdMediation(MengineAdMediation adMediation) {
        if (adMediation == MengineAdMediation.ADMEDIATION_APPLOVINMAX) {
            return "appLovin";
        }

        return "unknown";
    }

    @Override
    public void onMengineAdRevenue(MengineApplication application, MengineAdRevenueParam revenue) {
        if (m_firebaseAnalytics == null) {
            return;
        }

        String adFirebaseMediation = this.getAdMediation(revenue.ADREVENUE_MEDIATION);
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

    public void setUserProperty(@NonNull @Size(min = 1L,max = 24L) String name, @NonNull @Size(min = 1L,max = 36L) String value) {
        if (m_firebaseAnalytics == null) {
            return;
        }

        this.logMessage("setUserProperty name: %s value: %s"
            , name
            , value
        );

        m_firebaseAnalytics.setUserProperty(name, value);
    }

    public void logEvent(@NonNull @Size(min = 1L,max = 40L) String eventName, @Nullable Bundle params) {
        if (m_firebaseAnalytics == null) {
            return;
        }

        m_firebaseAnalytics.logEvent(eventName, params);
    }
}
