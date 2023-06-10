package org.Mengine.Plugin.FirebaseAnalytics;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.google.firebase.analytics.FirebaseAnalytics;

import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import java.util.Map;

public class MengineFirebaseAnalyticsPlugin extends MenginePlugin implements MenginePluginAnalyticsListener, MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "FirebaseAnalytics";
    public static final boolean PLUGIN_EMBEDDING = true;

    /**
     * void logEvent (String name, Bundle params)
     */

    FirebaseAnalytics m_firebaseAnalytics;

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        m_firebaseAnalytics = FirebaseAnalytics.getInstance(application);

        String installKey = application.getInstallKey();
        m_firebaseAnalytics.setUserId(installKey);

        long installKeyTimestamp = application.getInstallKeyTimestamp();
        m_firebaseAnalytics.setUserProperty("install_key_timestamp", String.valueOf(installKeyTimestamp));

        long sessionIndex = application.getSessionIndex();
        m_firebaseAnalytics.setUserProperty("session_index", String.valueOf(sessionIndex));
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        //Empty
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
                        this.logError("unsupported parameter: %s class: %s"
                            , value
                            , value.getClass()
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

                String VIRTUAL_CURRENCY_NAME = (String)parameters.get("@VIRTUAL_CURRENCY_NAME");
                Double VALUE = (Double)parameters.get("@VALUE");

                params.putString(FirebaseAnalytics.Param.VIRTUAL_CURRENCY_NAME, VIRTUAL_CURRENCY_NAME);
                params.putDouble(FirebaseAnalytics.Param.VALUE, VALUE);

                this.logInfo("logEvent [EARN_VIRTUAL_CURRENCY] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.EARN_VIRTUAL_CURRENCY, params);
            } break;
            case EAET_SPEND_VIRTUAL_CURRENCY: {
                Bundle params = new Bundle();

                String ITEM_NAME = (String)parameters.get("@ITEM_NAME");
                String VIRTUAL_CURRENCY_NAME = (String)parameters.get("@VIRTUAL_CURRENCY_NAME");
                Double VALUE = (Double)parameters.get("@VALUE");

                params.putString(FirebaseAnalytics.Param.ITEM_NAME, ITEM_NAME);
                params.putString(FirebaseAnalytics.Param.VIRTUAL_CURRENCY_NAME, VIRTUAL_CURRENCY_NAME);
                params.putDouble(FirebaseAnalytics.Param.VALUE, VALUE);

                this.logInfo("logEvent [SPEND_VIRTUAL_CURRENCY] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.SPEND_VIRTUAL_CURRENCY, params);
            } break;
            case EAET_UNLOCK_ACHIEVEMENT: {
                Bundle params = new Bundle();

                String ACHIEVEMENT_ID = (String)parameters.get("@ACHIEVEMENT_ID");

                params.putString(FirebaseAnalytics.Param.ACHIEVEMENT_ID, ACHIEVEMENT_ID);

                this.logInfo("logEvent [UNLOCK_ACHIEVEMENT] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.UNLOCK_ACHIEVEMENT, params);
            } break;
            case EAET_LEVEL_UP: {
                Bundle params = new Bundle();

                String CHARACTER = (String)parameters.get("@CHARACTER");
                Long LEVEL = (Long)parameters.get("@LEVEL");

                params.putString(FirebaseAnalytics.Param.CHARACTER, CHARACTER);
                params.putLong(FirebaseAnalytics.Param.LEVEL, LEVEL);

                this.logInfo("logEvent [LEVEL_UP] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.LEVEL_UP, params);
            } break;
            case EAET_LEVEL_START: {
                Bundle params = new Bundle();

                String LEVEL_NAME = (String)parameters.get("@LEVEL_NAME");

                params.putString(FirebaseAnalytics.Param.LEVEL_NAME, LEVEL_NAME);

                this.logInfo("logEvent [LEVEL_START] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.LEVEL_START, params);
            } break;
            case EAET_LEVEL_END: {
                Bundle params = new Bundle();

                String LEVEL_NAME = (String)parameters.get("@LEVEL_NAME");
                Boolean SUCCESS = (Boolean)parameters.get("@SUCCESS");

                params.putString(FirebaseAnalytics.Param.LEVEL_NAME, LEVEL_NAME);
                params.putBoolean(FirebaseAnalytics.Param.SUCCESS, SUCCESS);

                this.logInfo("logEvent [LEVEL_END] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.LEVEL_END, params);
            } break;
            case EAET_SELECT_ITEM: {
                Bundle params = new Bundle();

                String ITEM_LIST_ID = (String)parameters.get("@ITEM_LIST_ID");
                String ITEM_LIST_NAME = (String)parameters.get("@ITEM_LIST_NAME");

                params.putString(FirebaseAnalytics.Param.ITEM_LIST_ID, ITEM_LIST_ID);
                params.putString(FirebaseAnalytics.Param.ITEM_LIST_NAME, ITEM_LIST_NAME);

                this.logInfo("logEvent [SELECT_ITEM] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.SELECT_ITEM, params);
            } break;
            case EAET_TUTORIAL_BEGIN: {
                Bundle params = new Bundle();

                this.logInfo("logEvent [TUTORIAL_BEGIN] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.TUTORIAL_BEGIN, params);
            } break;
            case EAET_TUTORIAL_COMPLETE: {
                Bundle params = new Bundle();

                this.logInfo("logEvent [TUTORIAL_COMPLETE] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.TUTORIAL_COMPLETE, params);
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
