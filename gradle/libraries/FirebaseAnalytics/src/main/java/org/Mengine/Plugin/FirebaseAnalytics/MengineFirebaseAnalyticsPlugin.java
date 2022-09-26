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

    public void logEvent(@NonNull String name, @Nullable Bundle params) {
        mFirebaseAnalytics.logEvent(name, params);
    }
}
