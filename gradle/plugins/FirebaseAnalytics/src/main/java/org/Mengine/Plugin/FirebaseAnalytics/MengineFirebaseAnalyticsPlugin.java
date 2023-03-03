package org.Mengine.Plugin.FirebaseAnalytics;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.google.firebase.analytics.FirebaseAnalytics;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;

import java.util.Map;

public class MengineFirebaseAnalyticsPlugin extends MenginePlugin implements MenginePluginAnalyticsListener, MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "FirebaseAnalytics";
    public static final boolean PLUGIN_EMBEDDING = true;

    /**
     * void logEvent (String name, Bundle params)
     */

    FirebaseAnalytics m_firebaseAnalytics;

    @Override
    public void onAppCreate(MengineApplication application) {
        m_firebaseAnalytics = FirebaseAnalytics.getInstance(application);

        String installKey = application.getInstallKey();
        m_firebaseAnalytics.setUserId(installKey);

        long installKeyTimestamp = application.getInstallKeyTimestamp();
        m_firebaseAnalytics.setUserProperty("install_key_timestamp", String.valueOf(installKeyTimestamp));

        long sessionIndex = application.getSessionIndex();
        m_firebaseAnalytics.setUserProperty("session_index", String.valueOf(sessionIndex));
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
    public void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> parameters) {
        switch (eventType) {
            case EAET_CUSTOM: {
                Bundle params = new Bundle();

                for (Map.Entry<String, Object> entry : parameters.entrySet()) {
                    String name = entry.getKey();
                    Object value = entry.getValue();

                    if (value instanceof Boolean) {
                        params.putBoolean(name, (Boolean)value);
                    } else if (value instanceof Long) {
                        params.putLong(name, (Long)value);
                    } else if (value instanceof Double) {
                        params.putDouble(name, (Double)value);
                    } else if (value instanceof String) {
                        params.putString(name, (String)value);
                    } else {
                        this.logError("unsupported parameter [%s] %s"
                            , value.getClass()
                            , value
                        );

                        return;
                    }
                }

                this.logInfo("logEvent [CUSTOM] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(eventName, params);
            } break;
            case EAET_EARN_VIRTUAL_CURRENCY: {
                Bundle params = new Bundle();

                String virtualCurrencyName = (String)parameters.get("@INTERNAL_VIRTUAL_CURRENCY_NAME");
                Double value = (Double)parameters.get("@INTERNAL_VALUE");

                params.putString(FirebaseAnalytics.Param.VIRTUAL_CURRENCY_NAME, virtualCurrencyName);
                params.putDouble(FirebaseAnalytics.Param.VALUE, value);

                this.logInfo("logEvent [EARN_VIRTUAL_CURRENCY] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.EARN_VIRTUAL_CURRENCY, params);
            } break;
            case EAET_SPEND_VIRTUAL_CURRENCY: {
                Bundle params = new Bundle();

                String itemName = (String)parameters.get("@INTERNAL_ITEM_NAME");
                String virtualCurrencyName = (String)parameters.get("@INTERNAL_VIRTUAL_CURRENCY_NAME");
                Double value = (Double)parameters.get("@INTERNAL_VALUE");

                params.putString(FirebaseAnalytics.Param.ITEM_NAME, itemName);
                params.putString(FirebaseAnalytics.Param.VIRTUAL_CURRENCY_NAME, virtualCurrencyName);
                params.putDouble(FirebaseAnalytics.Param.VALUE, value);

                this.logInfo("logEvent [SPEND_VIRTUAL_CURRENCY] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.SPEND_VIRTUAL_CURRENCY, params);
            } break;
            case EAET_UNLOCK_ACHIEVEMENT: {
                Bundle params = new Bundle();

                String achievementId = (String)parameters.get("@INTERNAL_ACHIEVEMENT_ID");

                params.putString(FirebaseAnalytics.Param.ACHIEVEMENT_ID, achievementId);

                this.logInfo("logEvent [UNLOCK_ACHIEVEMENT] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.UNLOCK_ACHIEVEMENT, params);
            } break;
            default: {
                this.logWarning("event: %s unknown type: %d"
                    , eventName
                    , eventType
                );
            } break;
        }
    }

    public void logEvent(@NonNull String eventName, @Nullable Bundle params) {
        this.logMessage("logEvent eventName: %s params: %s"
            , eventName
            , params
        );

        m_firebaseAnalytics.logEvent(eventName, params);
    }
}
