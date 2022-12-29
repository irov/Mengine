package org.Mengine.Plugin.FirebaseAnalytics;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.Size;

import com.google.firebase.analytics.FirebaseAnalytics;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;

import java.util.HashMap;
import java.util.Map;

public class MengineFirebaseAnalyticsPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "FirebaseAnalytics";
    public static boolean PLUGIN_EMBEDDING = true;

    /**
     * void logEvent (String name, Bundle params)
     */

    FirebaseAnalytics mFirebaseAnalytics;

    @Override
    public void onAppCreate(MengineApplication application) {
        mFirebaseAnalytics = FirebaseAnalytics.getInstance(application);
        mFirebaseAnalytics.setAnalyticsCollectionEnabled(!BuildConfig.DEBUG);
    }

    @Override
    public void onMengineAnalyticsEvent(MengineActivity activity, String eventName, long timestamp, HashMap<String, Object> parameters) {
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
            }
        }

        this.logEvent(eventName, params);
    }

    public void logEvent(@NonNull String name, @Nullable Bundle params) {
        mFirebaseAnalytics.logEvent(name, params);
    }
}
