package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdWaterfallInfo;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxNetworkResponseInfo;

import com.applovin.sdk.AppLovinPrivacySettings;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;

import android.content.Context;
import android.os.Bundle;

import java.util.ArrayList;
import java.util.List;

public class MengineAppLovinPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "AppLovin";
    public static boolean PLUGIN_EMBEDDING = true;

    /**
     * <p>
     * void initialize()
     * - onApplovinPluginOnSdkInitialized
     * <p>
     * установка Interstitial
     * void initInterstitial()
     * void loadInterstitial()
     * void showInterstitial()
     * - onApplovinInterstitialOnAdLoaded
     * - onApplovinInterstitialOnAdDisplayed
     * - onApplovinInterstitialOnAdHidden
     * - onApplovinInterstitialOnAdClicked
     * - onApplovinInterstitialOnAdLoadFailed
     * - onApplovinInterstitialOnAdDisplayFailed
     * <p>
     * установка Rewarded
     * void initRewarded()
     * void loadRewarded()
     * void showRewarded()
     * - onApplovinRewardedOnRewardedVideoStarted
     * - onApplovinRewardedOnRewardedVideoCompleted
     * - onApplovinRewardedOnUserRewarded
     * - onApplovinRewardedOnAdLoaded
     * - onApplovinRewardedOnAdDisplayed
     * - onApplovinRewardedOnAdHidden
     * - onApplovinRewardedOnAdClicked
     * - onApplovinRewardedOnAdLoadFailed
     * - onApplovinRewardedOnAdDisplayFailed
     * <p>
     * установка Banner
     * void initBanner()
     * void bannerVisible(boolean show)
     * - onApplovinBannerOnAdDisplayed
     * - onApplovinBannerOnAdHidden
     * - onApplovinBannerOnAdClicked
     * - onApplovinBannerOnAdLoadFailed
     * - onApplovinBannerOnAdDisplayFailed
     * - onApplovinBannerOnAdExpanded
     * - onApplovinBannerOnAdCollapsed
     * <p>
     * void showMediationDebugger()
     */

    private MengineAppLovinBanner m_banner;
    private MengineAppLovinInterstitial m_interstitial;
    private MengineAppLovinRewarded m_rewarded;

    private MengineAppLovinMediationInterface m_mediationAmazon;

    private List<MengineAppLovinAnalyticsInterface> m_analytics;

    @Override
    public void onEvent(MengineActivity activity, String id, Object ... args) {
        if (id == "AdvertisingId") {
            final Context context = activity.getBaseContext();

            String advertisingId = (String)args[0];
            boolean advertisingLimitTrackingEnabled = (boolean)args[1];

            if (advertisingLimitTrackingEnabled == true) {
                this.logInfo("setHasUserConsent: false");

                AppLovinPrivacySettings.setHasUserConsent(false, context);
            } else {
                this.logInfo("setHasUserConsent: true");

                AppLovinPrivacySettings.setHasUserConsent(true, context);
            }
        }
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        final Context context = activity.getBaseContext();

        MengineAppLovinMediationInterface mediationAmazon = this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinMediationAmazon", false);

        if (mediationAmazon != null) {
            try {
                mediationAmazon.initializeMediator(activity);

                m_mediationAmazon = mediationAmazon;
            } catch (Exception e) {
            }
        }

        m_analytics = new ArrayList<MengineAppLovinAnalyticsInterface>();

        MengineAppLovinAnalyticsInterface firebaseAnalytics = this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinFirebaseAnalytics", false);

        if (firebaseAnalytics != null) {
            if (firebaseAnalytics.initializeAnalytics(this, activity) == true) {
                m_analytics.add(firebaseAnalytics);
            }
        }

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);

        appLovinSdk.setMediationProvider("max");

        boolean MengineAppLovinPlugin_VerboseLogging = activity.getMetaDataBoolean("mengine.applovin.verbose_logging", false);

        if (MengineAppLovinPlugin_VerboseLogging == true) {
            this.logInfo("setVerboseLogging: true");

            appLovinSdk.getSettings().setVerboseLogging(true);
        }

        boolean MengineAppLovinPlugin_IsAgeRestrictedUser = activity.getMetaDataBoolean("mengine.applovin.is_age_restricted_user", true);

        this.logInfo("setIsAgeRestrictedUser: %b"
            , MengineAppLovinPlugin_IsAgeRestrictedUser
        );

        AppLovinPrivacySettings.setIsAgeRestrictedUser(MengineAppLovinPlugin_IsAgeRestrictedUser, context);

        boolean MengineAppLovinPlugin_CCPA = activity.getMetaDataBoolean("mengine.applovin.CCPA", true);

        this.logInfo("CCPA: %b"
            , MengineAppLovinPlugin_CCPA
        );

        AppLovinPrivacySettings.setDoNotSell(MengineAppLovinPlugin_CCPA, context);

        AppLovinSdk.initializeSdk(context, new AppLovinSdk.SdkInitializationListener() {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration) {
                MengineAppLovinPlugin.this.logInfo("AppLovinSdk initialized: country [%s]"
                    , configuration.getCountryCode()
                );

                MengineAppLovinPlugin.this.activateSemaphore("AppLovinSdkInitialized");
            }
        });

        if (BuildConfig.DEBUG == true) {
            this.showMediationDebugger();
        }
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        if (m_mediationAmazon != null) {
            m_mediationAmazon = null;
        }

        if( m_analytics != null ) {
            for (MengineAppLovinAnalyticsInterface analytic : m_analytics) {
                analytic.finalizeAnalytics(this);
            }

            m_analytics = null;
        }

        if (m_interstitial != null) {
            m_interstitial.destroy();
            m_interstitial = null;
        }

        if (m_rewarded != null) {
            m_rewarded.destroy();
            m_rewarded = null;
        }

        if (m_banner != null) {
            m_banner.destroy();
            m_banner = null;
        }
    }

    public MengineAppLovinMediationInterface getMediationAmazon() {
        return m_mediationAmazon;
    }

    public void initBanner(String adUnitId) throws Exception {
        m_banner = new MengineAppLovinBanner(this, adUnitId);
    }

    public void bannerVisible(boolean show) {
        m_banner.bannerVisible(show);
    }

    public void initInterstitial(String adUnitId) throws Exception {
        m_interstitial = new MengineAppLovinInterstitial(this, adUnitId);
    }

    public void loadInterstitial() {
        m_interstitial.loadInterstitial();
    }

    public void showInterstitial() {
        m_interstitial.showInterstitial();
    }

    public void initRewarded(String adUnitId) throws Exception {
        m_rewarded = new MengineAppLovinRewarded(this, adUnitId);
    }

    public void loadRewarded() {
        m_rewarded.loadRewarded();
    }

    public void showRewarded() {
        m_rewarded.showRewarded();
    }

    public void onEventRevenuePaid(MaxAd ad) {
        for (MengineAppLovinAnalyticsInterface analytic : m_analytics) {
            analytic.onEventRevenuePaid(this, ad);
        }
    }

    public void showMediationDebugger() {
        MengineActivity activity = this.getActivity();
        final Context context = activity.getBaseContext();

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);
        appLovinSdk.showMediationDebugger();
    }
}
