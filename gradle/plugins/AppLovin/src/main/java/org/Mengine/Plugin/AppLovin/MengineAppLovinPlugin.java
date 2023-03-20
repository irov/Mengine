package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginExtension;
import org.Mengine.Base.MenginePluginExtensionListener;

import com.applovin.mediation.MaxAd;

import com.applovin.sdk.AppLovinPrivacySettings;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;

import android.content.Context;
import android.os.Bundle;

import java.util.ArrayList;

public class MengineAppLovinPlugin extends MenginePlugin implements MenginePluginExtensionListener {
    public static final String PLUGIN_NAME = "AppLovin";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_IS_AGE_RESTRICTED_USER = "mengine.applovin.is_age_restricted_user";
    public static final String PLUGIN_METADATA_CCPA = "mengine.applovin.CCPA";

    /**
     * <p>
     * void initialize()
     * - onApplovinPluginOnSdkInitialized
     * <p>
     * установка Interstitial
     * void initInterstitial()
     * boolean canYouShowInterstitial()
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
     * boolean canOfferRewarded()
     * boolean canYouShowRewarded()
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

    private ArrayList<MengineAppLovinAnalyticsListener> m_analytics = new ArrayList<>();

    @Override
    public void onEvent(MengineActivity activity, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_ADVERTISING_ID) {
            final Context context = activity.getBaseContext();

            String advertisingId = (String)args[0];
            boolean advertisingLimitTrackingEnabled = (boolean)args[1];

            if (advertisingLimitTrackingEnabled == true) {
                this.logMessage("setHasUserConsent: false");

                AppLovinPrivacySettings.setHasUserConsent(false, context);
            } else {
                this.logMessage("setHasUserConsent: true");

                AppLovinPrivacySettings.setHasUserConsent(true, context);
            }
        }
    }

    @Override
    public void onMenginePluginExtension(MengineApplication application, MengineActivity activity, MenginePlugin plugin, MenginePluginExtension extension) {
        if (extension instanceof MengineAppLovinAnalyticsListener) {
            MengineAppLovinAnalyticsListener listener = (MengineAppLovinAnalyticsListener)extension;

            m_analytics.add(listener);
        }
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        final Context context = activity.getBaseContext();

        MengineAppLovinMediationInterface mediationAmazon = this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinMediationAmazon", false);

        if (mediationAmazon != null) {
            mediationAmazon.initializeMediator(activity, this);

            m_mediationAmazon = mediationAmazon;
        }

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);

        appLovinSdk.setMediationProvider("max");

        MengineApplication application = activity.getMengineApplication();
        String installKey = application.getInstallKey();

        appLovinSdk.setUserIdentifier(installKey);

        boolean MengineAppLovinPlugin_IsAgeRestrictedUser = activity.getMetaDataBoolean(PLUGIN_METADATA_IS_AGE_RESTRICTED_USER, true);

        this.logMessage("setIsAgeRestrictedUser: %b"
            , MengineAppLovinPlugin_IsAgeRestrictedUser
        );

        AppLovinPrivacySettings.setIsAgeRestrictedUser(MengineAppLovinPlugin_IsAgeRestrictedUser, context);

        boolean MengineAppLovinPlugin_CCPA = activity.getMetaDataBoolean(PLUGIN_METADATA_CCPA, true);

        this.logMessage("CCPA: %b"
            , MengineAppLovinPlugin_CCPA
        );

        AppLovinPrivacySettings.setDoNotSell(MengineAppLovinPlugin_CCPA, context);

        AppLovinSdk.initializeSdk(context, new AppLovinSdk.SdkInitializationListener() {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration) {
                MengineAppLovinPlugin.this.logMessage("AppLovinSdk initialized: country [%s] AmazonAdUnitIds: %s"
                    , configuration.getCountryCode()
                    , configuration.getEnabledAmazonAdUnitIds()
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
            m_mediationAmazon.finalizeMediator(activity, this);
            m_mediationAmazon = null;
        }

        m_analytics = null;

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

    public void initBanner(String adUnitId) {
        m_banner = new MengineAppLovinBanner(this, adUnitId);
    }

    public void bannerVisible(boolean show) {
        m_banner.bannerVisible(show);
    }

    public void initInterstitial(String adUnitId) {
        m_interstitial = new MengineAppLovinInterstitial(this, adUnitId);
    }

    public boolean canYouShowInterstitial() {
        return m_interstitial.canYouShowInterstitial();
    }

    public boolean showInterstitial() {
        return m_interstitial.showInterstitial();
    }

    public void initRewarded(String adUnitId) {
        m_rewarded = new MengineAppLovinRewarded(this, adUnitId);
    }

    public boolean canOfferRewarded() {
        return m_rewarded.canOfferRewarded();
    }

    public boolean canYouShowRewarded() {
        return m_rewarded.canYouShowRewarded();
    }

    public boolean showRewarded() {
        return m_rewarded.showRewarded();
    }

    public void onEventRevenuePaid(MaxAd ad) {
        for (MengineAppLovinAnalyticsListener analytic : m_analytics) {
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
