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
        if(id == "ConsentStatus") {
            final Context context = activity.getBaseContext();

            Integer consent = (Integer)args[0];

            switch (consent) {
                case -1:
                case 0:
                case 1:
                    this.logInfo("setHasUserConsent: false");

                    AppLovinPrivacySettings.setHasUserConsent(false, context);
                    break;
                case 2:
                    this.logInfo("setHasUserConsent: true");

                    AppLovinPrivacySettings.setHasUserConsent(true, context);
                    break;
            }
        }
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        if (m_mediationAmazon != null) {
            m_mediationAmazon = null;
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

    public void initialize() {
        MengineActivity activity = this.getActivity();
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
            if (firebaseAnalytics.initializeAnalytics(activity) == true) {
                m_analytics.add(firebaseAnalytics);
            }
        }

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);

        appLovinSdk.setMediationProvider("max");

        boolean OPTION_applovinverbose = activity.hasOption("applovinverbose");
        boolean AppLovinPlugin_VerboseLogging = activity.getConfigValueBoolean("AppLovinPlugin", "VerboseLogging", false);

        if (OPTION_applovinverbose == true || AppLovinPlugin_VerboseLogging == true) {
            this.logInfo("setVerboseLogging: true");

            appLovinSdk.getSettings().setVerboseLogging(true);
        }

        boolean AppLovinPlugin_IsAgeRestrictedUser = activity.getConfigValueBoolean("AppLovinPlugin", "IsAgeRestrictedUser", true);

        this.logInfo("setIsAgeRestrictedUser: %b"
            , AppLovinPlugin_IsAgeRestrictedUser
        );

        AppLovinPrivacySettings.setIsAgeRestrictedUser( AppLovinPlugin_IsAgeRestrictedUser, context );

        boolean AppLovinPlugin_CCPA = activity.getConfigValueBoolean("AppLovinPlugin", "CCPA", true);

        this.logInfo("CCPA: %b"
            , AppLovinPlugin_CCPA
        );

        AppLovinPrivacySettings.setDoNotSell( AppLovinPlugin_CCPA, context );

        AppLovinSdk.initializeSdk(context, new AppLovinSdk.SdkInitializationListener() {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration) {
                MengineAppLovinPlugin.this.logInfo("AppLovinSdk initialized: country [%s]"
                    , configuration.getCountryCode()
                );

                MengineAppLovinPlugin.this.pythonCall("onApplovinPluginOnSdkInitialized");
            }
        });

        boolean OPTION_applovindebugger = activity.hasOption("applovindebugger");

        if (OPTION_applovindebugger == true) {
            this.showMediationDebugger();
        }
    }

    public MengineAppLovinMediationInterface getMediationAmazon() {
        return m_mediationAmazon;
    }

    public void initBanner() throws Exception {
        m_banner = new MengineAppLovinBanner(this);
    }

    public void bannerVisible(boolean show) {
        m_banner.bannerVisible(show);
    }

    public void initInterstitial() throws Exception {
        m_interstitial = new MengineAppLovinInterstitial(this);
    }

    public void loadInterstitial() {
        m_interstitial.loadInterstitial();
    }

    public void showInterstitial() {
        m_interstitial.showInterstitial();
    }

    public void initRewarded() throws Exception {
        m_rewarded = new MengineAppLovinRewarded(this);
    }

    public void loadRewarded() {
        m_rewarded.loadRewarded();
    }

    public void showRewarded() {
        m_rewarded.showRewarded();
    }

    public void eventRevenuePaid(MaxAd ad) {
        for (MengineAppLovinAnalyticsInterface analytic : m_analytics) {
            analytic.eventRevenuePaid(ad);
        }
    }

    public void showMediationDebugger() {
        MengineActivity activity = this.getActivity();
        final Context context = activity.getBaseContext();

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);
        appLovinSdk.showMediationDebugger();
    }
}
