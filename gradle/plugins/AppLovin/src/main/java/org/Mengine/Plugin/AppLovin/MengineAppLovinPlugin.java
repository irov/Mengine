package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginExtension;
import org.Mengine.Base.MenginePluginExtensionListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import com.applovin.mediation.MaxAd;

import com.applovin.sdk.AppLovinPrivacySettings;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;

import android.content.Context;
import android.os.Bundle;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

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
     * void initInterstitial(String adUnitId)
     * boolean canYouShowInterstitial(String adUnitId)
     * void showInterstitial(String adUnitId)
     * - onApplovinInterstitialOnAdLoaded
     * - onApplovinInterstitialOnAdDisplayed
     * - onApplovinInterstitialOnAdHidden
     * - onApplovinInterstitialOnAdClicked
     * - onApplovinInterstitialOnAdLoadFailed
     * - onApplovinInterstitialOnAdDisplayFailed
     * <p>
     * установка Rewarded
     * void initRewarded(String adUnitId)
     * boolean canOfferRewarded(String adUnitId)
     * boolean canYouShowRewarded(String adUnitId)
     * void showRewarded(String adUnitId)
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
     * void initBanner(String adUnitId)
     * void bannerVisible(String adUnitId, boolean show)
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

    private Map<String, MengineAppLovinBanner> m_banners = new HashMap<>();
    private Map<String, MengineAppLovinInterstitial> m_interstitials = new HashMap<>();
    private Map<String, MengineAppLovinRewarded> m_rewardeds = new HashMap<>();

    private MengineAppLovinMediationInterface m_mediationAmazon;

    private ArrayList<MengineAppLovinAnalyticsListener> m_analytics = new ArrayList<>();

    @Override
    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_ADVERTISING_ID) {
            final Context context = application.getApplicationContext();

            String advertisingId = (String)args[0];
            boolean advertisingLimitTrackingEnabled = (boolean)args[1];

            if (advertisingLimitTrackingEnabled == true) {
                this.logMessage("setHasUserConsent: false");

                AppLovinPrivacySettings.setHasUserConsent(false, context);
            } else {
                this.logMessage("setHasUserConsent: true");

                AppLovinPrivacySettings.setHasUserConsent(true, context);
            }
        } else if (event == MengineEvent.EVENT_SESSION_ID) {
            Context context = application.getApplicationContext();
            AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);

            String sessionId = (String)args[0];

            appLovinSdk.setUserIdentifier(sessionId);
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
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        final Context context = activity.getApplicationContext();

        MengineAppLovinMediationInterface mediationAmazon = this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinMediationAmazon", false);

        if (mediationAmazon != null) {
            mediationAmazon.initializeMediator(activity, this);

            m_mediationAmazon = mediationAmazon;
        }

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);

        appLovinSdk.setMediationProvider("max");

        MengineApplication application = activity.getMengineApplication();
        String sessionId = application.getSessionId();

        appLovinSdk.setUserIdentifier(sessionId);

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
                MengineAppLovinPlugin.this.logMessage("AppLovinSdk initialized country: %s AmazonAdUnitIds: %s"
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

        for( MengineAppLovinBanner banner : m_banners.values() ) {
            banner.destroy();
        }

        m_banners = null;

        for( MengineAppLovinInterstitial interstitial : m_interstitials.values() ) {
            interstitial.destroy();
        }

        m_interstitials = null;

        for( MengineAppLovinRewarded rewarded : m_rewardeds.values() ) {
            rewarded.destroy();
        }

        m_rewardeds = null;
    }

    public MengineAppLovinMediationInterface getMediationAmazon() {
        return m_mediationAmazon;
    }

    private void assertBanner(String adUnitId) {
        if (m_banners.containsKey(adUnitId) == false) {
            this.assertionError("not found banner: %s", adUnitId);
        }
    }

    public void initBanner(String adUnitId) {
        if (m_banners.containsKey(adUnitId) == false) {
            this.assertionError("already exist banner: %s", adUnitId);
        }

        MengineAppLovinBanner banner = new MengineAppLovinBanner(this, adUnitId);

        m_banners.put(adUnitId, banner);
    }

    public void bannerVisible(String adUnitId, boolean show) {
        this.assertBanner(adUnitId);

        MengineAppLovinBanner banner = m_banners.get(adUnitId);

        banner.bannerVisible(show);
    }

    private void assertInterstitial(String adUnitId) {
        if (m_interstitials.containsKey(adUnitId) == false) {
            this.assertionError("not found interstitial: %s", adUnitId);
        }
    }

    public void initInterstitial(String adUnitId) {
        if (m_interstitials.containsKey(adUnitId) == true) {
            this.assertionError("already exist interstitial: %s", adUnitId);
        }

        MengineAppLovinInterstitial interstitial = new MengineAppLovinInterstitial(this, adUnitId);

        m_interstitials.put(adUnitId, interstitial);
    }

    public boolean canYouShowInterstitial(String adUnitId) {
        this.assertInterstitial(adUnitId);

        MengineAppLovinInterstitial interstitial = m_interstitials.get(adUnitId);

        boolean result = interstitial.canYouShowInterstitial();

        return result;
    }

    public boolean showInterstitial(String adUnitId) {
        this.assertInterstitial(adUnitId);

        MengineAppLovinInterstitial interstitial = m_interstitials.get(adUnitId);

        boolean result = interstitial.showInterstitial();

        return result;
    }

    public void initRewarded(String adUnitId) {
        if (m_rewardeds.containsKey(adUnitId) == true) {
            this.assertionError("already init rewarded: %s", adUnitId);
        }

        MengineAppLovinRewarded rewarded = new MengineAppLovinRewarded(this, adUnitId);

        m_rewardeds.put(adUnitId, rewarded);
    }

    private void assertRewarded(String adUnitId) {
        if (m_rewardeds.containsKey(adUnitId) == false) {
            this.assertionError("not found rewarded: %s", adUnitId);
        }
    }

    public boolean canOfferRewarded(String adUnitId) {
        this.assertRewarded(adUnitId);

        MengineAppLovinRewarded rewarded = m_rewardeds.get(adUnitId);

        boolean result = rewarded.canOfferRewarded();

        return result;
    }

    public boolean canYouShowRewarded(String adUnitId) {
        this.assertRewarded(adUnitId);

        MengineAppLovinRewarded rewarded = m_rewardeds.get(adUnitId);

        boolean result = rewarded.canYouShowRewarded();

        return result;
    }

    public boolean showRewarded(String adUnitId) {
        this.assertRewarded(adUnitId);

        MengineAppLovinRewarded rewarded = m_rewardeds.get(adUnitId);

        boolean result = rewarded.showRewarded();

        return result;
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
