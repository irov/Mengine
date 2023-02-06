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
    public void onMengineAnalyticsEvent(MengineActivity activity, String eventName, long timestamp, Map<String, Object> parameters) {
        Bundle params = new Bundle();

        for (Map.Entry<String, Object> entry : parameters.entrySet()) {
            String name = entry.getKey();
            Object parameter = entry.getValue();
            if (parameter instanceof Boolean) {
                params.putBoolean(name, (Boolean)parameter);
            } else if (parameter instanceof Long) {
                params.putLong(name, (Long)parameter);
            } else if (parameter instanceof Double) {
                params.putDouble(name, (Double)parameter);
            } else if (parameter instanceof String) {
                params.putString(name, (String)parameter);
            } else {
                this.logError("onMengineAnalyticsEvent: unsupported parameter [%s] %s"
                    , parameter.getClass()
                    , parameter
                );

                return;
            }
        }

        m_firebaseAnalytics.logEvent(eventName, params);
    }

    public void logEvent(@NonNull String eventName, @Nullable Bundle params) {
        this.logInfo("logEvent eventName: %s params: %s"
            , eventName
            , params
        );

        m_firebaseAnalytics.logEvent(eventName, params);
    }
}
