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
    public static String PLUGIN_NAME = "FirebaseAnalytics";
    public static boolean PLUGIN_EMBEDDING = true;

    /**
     * void logEvent (String name, Bundle params)
     */

    FirebaseAnalytics m_firebaseAnalytics;

    @Override
    public void onAppCreate(MengineApplication application) {
        m_firebaseAnalytics = FirebaseAnalytics.getInstance(application);
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
    public void onMengineAnalyticsEvent(MengineActivity activity, int eventType, String eventName, long timestamp, Map<String, Object> parameters) {
        Bundle params = new Bundle();

        for (Map.Entry<String, Object> entry : parameters.entrySet()) {
            String name = entry.getKey();

            if (name.contains("@INTERNAL_") == true) {
                if (eventType == EAET_CUSTOM) {
                    this.logError("incorrect custom event: %s parameter: %s [INTERNAL]"
                        , eventName
                        , name
                    );

                    return;
                }

                continue;
            }

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

        switch (eventType) {
            case EAET_CUSTOM: {
                m_firebaseAnalytics.logEvent(eventName, params);
            } break;
            case EAET_EARN_VIRTUAL_CURRENCY: {
                String virtualCurrencyName = (String)parameters.get("@INTERNAL_VIRTUAL_CURRENCY_NAME");
                Double value = (Double)parameters.get("@INTERNAL_VALUE");

                params.putString(FirebaseAnalytics.Param.VIRTUAL_CURRENCY_NAME, virtualCurrencyName);
                params.putDouble(FirebaseAnalytics.Param.VALUE, value);

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.EARN_VIRTUAL_CURRENCY, params);
            } break;
            case EAET_SPEND_VIRTUAL_CURRENCY: {
                String itemName = (String)parameters.get("@INTERNAL_ITEM_NAME");
                String virtualCurrencyName = (String)parameters.get("@INTERNAL_VIRTUAL_CURRENCY_NAME");
                Double value = (Double)parameters.get("@INTERNAL_VALUE");

                params.putString(FirebaseAnalytics.Param.ITEM_NAME, itemName);
                params.putString(FirebaseAnalytics.Param.VIRTUAL_CURRENCY_NAME, virtualCurrencyName);
                params.putDouble(FirebaseAnalytics.Param.VALUE, value);

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.SPEND_VIRTUAL_CURRENCY, params);
            } break;
            case EAET_UNLOCK_ACHIEVEMENT: {
                String achievementId = (String)parameters.get("@INTERNAL_ACHIEVEMENT_ID");

                params.putString(FirebaseAnalytics.Param.ACHIEVEMENT_ID, achievementId);

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.UNLOCK_ACHIEVEMENT, params);
            } break;
            default: {
                this.logError("Unknown event type: %d name: %s"
                    , eventType
                    , eventName
                );
            } break;
        }
    }

    public void logEvent(@NonNull String eventName, @Nullable Bundle params) {
        this.logInfo("logEvent eventName: %s params: %s"
            , eventName
            , params
        );

        m_firebaseAnalytics.logEvent(eventName, params);
    }
}
