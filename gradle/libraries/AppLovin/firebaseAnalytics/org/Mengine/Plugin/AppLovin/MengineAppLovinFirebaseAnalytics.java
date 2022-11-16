package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Plugin.AppLovin.MengineAppLovinAnalyticsInterface;
import org.Mengine.Plugin.FirebaseAnalytics.MengineFirebaseAnalyticsPlugin;

import com.google.firebase.analytics.FirebaseAnalytics;

import com.applovin.mediation.MaxAd;

import android.os.Bundle;

public class MengineAppLovinFirebaseAnalytics implements MengineAppLovinAnalyticsInterface {
    MengineFirebaseAnalyticsPlugin m_firebaseAnalyticsPlugin;

    public boolean initializeAnalytics(MengineActivity activity) {
        MengineFirebaseAnalyticsPlugin firebaseAnalyticsPlugin = activity.findPlugin(MengineFirebaseAnalyticsPlugin.class);

        if (firebaseAnalyticsPlugin == null) {
            return false;
        }

        m_firebaseAnalyticsPlugin = firebaseAnalyticsPlugin;

        return  true;
    }

    public void eventRevenuePaid(MaxAd ad) {
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
