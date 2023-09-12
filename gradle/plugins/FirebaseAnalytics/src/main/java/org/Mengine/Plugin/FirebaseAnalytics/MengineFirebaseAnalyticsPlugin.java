package org.Mengine.Plugin.FirebaseAnalytics;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.google.firebase.analytics.FirebaseAnalytics;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginAnalyticsListener;
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
    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_SESSION_ID) {
            String sessionId = (String)args[0];

            m_firebaseAnalytics.setUserId(sessionId);
        }
    }

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        m_firebaseAnalytics = FirebaseAnalytics.getInstance(application);

        String installKey = application.getInstallKey();
        long installTimestamp = application.getInstallTimestamp();
        String installVersion = application.getInstallVersion();
        long installRND = application.getInstallRND();
        long sessionIndex = application.getSessionIndex();
        long sessionTimestamp = application.getSessionTimestamp();

        Bundle params = new Bundle();
        params.putString("install_key", installKey);
        params.putLong("install_timestamp", installTimestamp);
        params.putString("install_version", installVersion);
        params.putLong("install_rnd", installRND);
        params.putLong("session_index", sessionIndex);
        params.putLong("session_timestamp", sessionTimestamp);

        m_firebaseAnalytics.setDefaultEventParameters(params);

        m_firebaseAnalytics.setUserProperty("is_dev", String.valueOf(BuildConfig.DEBUG));
        m_firebaseAnalytics.setUserProperty("install_key", installKey);
        m_firebaseAnalytics.setUserProperty("install_timestamp", String.valueOf(installTimestamp));
        m_firebaseAnalytics.setUserProperty("install_rnd", String.valueOf(installRND));
        m_firebaseAnalytics.setUserProperty("session_index", String.valueOf(sessionIndex));
        m_firebaseAnalytics.setUserProperty("session_timestamp", String.valueOf(sessionTimestamp));
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        //Empty
    }

    @Override
    public void onAppTerminate(MengineApplication application) {
        m_firebaseAnalytics = null;
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        if (m_firebaseAnalytics == null) {
            return;
        }

        switch (eventType) {
            case EAET_CUSTOM: {
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
                        this.logError("unsupported parameter: %s class: %s"
                            , value
                            , value.getClass()
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
                        this.logError("unsupported parameter: %s class: %s"
                            , value
                            , value.getClass()
                        );

                        return;
                    }
                }

                m_firebaseAnalytics.logEvent(eventName, params);
            } break;
            case EAET_EARN_VIRTUAL_CURRENCY: {
                Bundle params = new Bundle();

                String VIRTUAL_CURRENCY_NAME = (String)parameters.get("@VIRTUAL_CURRENCY_NAME");
                Double VALUE = (Double)parameters.get("@VALUE");

                params.putString(FirebaseAnalytics.Param.VIRTUAL_CURRENCY_NAME, VIRTUAL_CURRENCY_NAME);
                params.putDouble(FirebaseAnalytics.Param.VALUE, VALUE);

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

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.SPEND_VIRTUAL_CURRENCY, params);
            } break;
            case EAET_UNLOCK_ACHIEVEMENT: {
                Bundle params = new Bundle();

                String ACHIEVEMENT_ID = (String)parameters.get("@ACHIEVEMENT_ID");

                params.putString(FirebaseAnalytics.Param.ACHIEVEMENT_ID, ACHIEVEMENT_ID);

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.UNLOCK_ACHIEVEMENT, params);
            } break;
            case EAET_LEVEL_UP: {
                Bundle params = new Bundle();

                String CHARACTER = (String)parameters.get("@CHARACTER");
                Long LEVEL = (Long)parameters.get("@LEVEL");

                params.putString(FirebaseAnalytics.Param.CHARACTER, CHARACTER);
                params.putLong(FirebaseAnalytics.Param.LEVEL, LEVEL);

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.LEVEL_UP, params);
            } break;
            case EAET_LEVEL_START: {
                Bundle params = new Bundle();

                String LEVEL_NAME = (String)parameters.get("@LEVEL_NAME");

                params.putString(FirebaseAnalytics.Param.LEVEL_NAME, LEVEL_NAME);

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.LEVEL_START, params);
            } break;
            case EAET_LEVEL_END: {
                Bundle params = new Bundle();

                String LEVEL_NAME = (String)parameters.get("@LEVEL_NAME");
                Boolean SUCCESS = (Boolean)parameters.get("@SUCCESS");

                params.putString(FirebaseAnalytics.Param.LEVEL_NAME, LEVEL_NAME);
                params.putBoolean(FirebaseAnalytics.Param.SUCCESS, SUCCESS);

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.LEVEL_END, params);
            } break;
            case EAET_SELECT_ITEM: {
                Bundle params = new Bundle();

                String ITEM_LIST_ID = (String)parameters.get("@ITEM_LIST_ID");
                String ITEM_LIST_NAME = (String)parameters.get("@ITEM_LIST_NAME");

                params.putString(FirebaseAnalytics.Param.ITEM_LIST_ID, ITEM_LIST_ID);
                params.putString(FirebaseAnalytics.Param.ITEM_LIST_NAME, ITEM_LIST_NAME);

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.SELECT_ITEM, params);
            } break;
            case EAET_TUTORIAL_BEGIN: {
                Bundle params = new Bundle();

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.TUTORIAL_BEGIN, params);
            } break;
            case EAET_TUTORIAL_COMPLETE: {
                Bundle params = new Bundle();

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.TUTORIAL_COMPLETE, params);
            } break;
            case EAET_SCREEN_VIEW: {
                Bundle params = new Bundle();

                String SCREEN_CLASS = (String)parameters.get("@SCREEN_CLASS");
                String SCREEN_NAME = (String)parameters.get("@SCREEN_NAME");

                params.putString(FirebaseAnalytics.Param.SCREEN_CLASS, SCREEN_CLASS);
                params.putString(FirebaseAnalytics.Param.SCREEN_NAME, SCREEN_NAME);

                m_firebaseAnalytics.logEvent(FirebaseAnalytics.Event.SCREEN_VIEW, params);
            } break;
            default: {
                this.logWarning("event: %s unknown type: %d"
                    , eventName
                    , eventType
                );
            } break;
        }
    }

    @Override
    public void onMengineAnalyticsFlush(MengineApplication application) {
        if (m_firebaseAnalytics == null) {
            return;
        }

        //Empty
    }

    public void setUserProperty(@NonNull String name, @NonNull String value) {
        if (m_firebaseAnalytics == null) {
            return;
        }

        this.logMessage("setUserProperty name: %s value: %s"
            , name
            , value
        );

        m_firebaseAnalytics.setUserProperty(name, value);
    }

    public void logEvent(@NonNull String eventName, @Nullable Bundle params) {
        if (m_firebaseAnalytics == null) {
            return;
        }

        m_firebaseAnalytics.logEvent(eventName, params);
    }
}
