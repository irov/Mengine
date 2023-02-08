package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import org.Mengine.Plugin.FirebaseAnalytics.MengineFirebaseAnalyticsPlugin;

import com.google.firebase.analytics.FirebaseAnalytics;

import com.applovin.mediation.MaxAd;

import android.os.Bundle;

public class MengineAppLovinFirebaseAnalytics implements MengineAppLovinAnalyticsInterface {
    MengineFirebaseAnalyticsPlugin m_firebaseAnalyticsPlugin;

    @Override
    public boolean initializeAnalytics(MenginePlugin plugin, MengineActivity activity) {
        MengineFirebaseAnalyticsPlugin firebaseAnalyticsPlugin = activity.getPlugin(MengineFirebaseAnalyticsPlugin.class);

        if (firebaseAnalyticsPlugin == null) {
            return false;
        }

        m_firebaseAnalyticsPlugin = firebaseAnalyticsPlugin;

        return  true;
    }

    @Override
    public void finalizeAnalytics(MenginePlugin plugin) {
        m_firebaseAnalyticsPlugin = null;
    }

    @Override
    public void onEventRevenuePaid(MenginePlugin plugin, MaxAd ad) {
        Bundle params = new Bundle();
        params.putString(FirebaseAnalytics.Param.AD_PLATFORM, "appLovin");
        params.putString(FirebaseAnalytics.Param.AD_SOURCE, ad.getNetworkName());
        params.putString(FirebaseAnalytics.Param.AD_FORMAT, ad.getFormat().getLabel());
        params.putString(FirebaseAnalytics.Param.AD_UNIT_NAME, ad.getAdUnitId());
        params.putDouble(FirebaseAnalytics.Param.VALUE, ad.getRevenue());
        params.putString(FirebaseAnalytics.Param.CURRENCY, "USD");

        m_firebaseAnalyticsPlugin.logEvent(FirebaseAnalytics.Event.AD_IMPRESSION, params);
    }
}
