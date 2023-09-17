package org.Mengine.Plugin.AppLovin;

import android.content.Context;
import android.os.Bundle;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.sdk.AppLovinPrivacySettings;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdRevenueParam;
import org.Mengine.Base.MengineAnalytics;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginExtension;
import org.Mengine.Base.MenginePluginExtensionListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class MengineAppLovinPlugin extends MenginePlugin implements MenginePluginExtensionListener, MenginePluginActivityListener {
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
     * boolean canYouShowInterstitial(String adUnitId, String placement)
     * void showInterstitial(String adUnitId, String placement)
     * - onApplovinInterstitialOnAdLoaded
     * - onApplovinInterstitialOnAdDisplayed
     * - onApplovinInterstitialOnAdHidden
     * - onApplovinInterstitialOnAdClicked
     * - onApplovinInterstitialOnAdLoadFailed
     * - onApplovinInterstitialOnAdDisplayFailed
     * <p>
     * установка Rewarded
     * void initRewarded(String adUnitId)
     * boolean canOfferRewarded(String adUnitId, String placement)
     * boolean canYouShowRewarded(String adUnitId, String placement)
     * void showRewarded(String adUnitId, String placement)
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

    private Map<String, MengineAppLovinBanner> m_banners;
    private Map<String, MengineAppLovinInterstitial> m_interstitials;
    private Map<String, MengineAppLovinRewarded> m_rewardeds;

    private MengineAppLovinMediationInterface m_mediationAmazon;

    private ArrayList<MengineAppLovinAnalyticsListener> m_analytics;

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
        m_banners = new HashMap<>();
        m_interstitials = new HashMap<>();
        m_rewardeds = new HashMap<>();

        m_analytics = new ArrayList<>();

        final Context context = activity.getApplicationContext();

        MengineAppLovinMediationInterface mediationAmazon = this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinMediationAmazon", false);

        if (mediationAmazon != null) {
            mediationAmazon.initializeMediator(activity, this);

            m_mediationAmazon = mediationAmazon;
        }

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);

        appLovinSdk.setMediationProvider("max");

        boolean MengineAppLovinPlugin_IsAgeRestrictedUser = activity.getMetaDataBoolean(PLUGIN_METADATA_IS_AGE_RESTRICTED_USER, false);

        this.logMessage("%s: %b"
            , PLUGIN_METADATA_IS_AGE_RESTRICTED_USER
            , MengineAppLovinPlugin_IsAgeRestrictedUser
        );

        AppLovinPrivacySettings.setIsAgeRestrictedUser(MengineAppLovinPlugin_IsAgeRestrictedUser, context);

        boolean MengineAppLovinPlugin_CCPA = activity.getMetaDataBoolean(PLUGIN_METADATA_CCPA, true);

        this.logMessage("%s: %b"
            , PLUGIN_METADATA_CCPA
            , MengineAppLovinPlugin_CCPA
        );

        AppLovinPrivacySettings.setDoNotSell(MengineAppLovinPlugin_CCPA, context);

        if (BuildConfig.DEBUG == true) {
            appLovinSdk.getSettings().setCreativeDebuggerEnabled(true);
        } else {
            appLovinSdk.getSettings().setCreativeDebuggerEnabled(false);
        }

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

    public void initBanner(String adUnitId, String placement) {
        if (m_banners.containsKey(adUnitId) == false) {
            this.assertionError("already exist banner: %s", adUnitId);
        }

        MengineAppLovinBanner banner = new MengineAppLovinBanner(this, adUnitId, placement);

        m_banners.put(adUnitId, banner);
    }

    public boolean bannerVisible(String adUnitId, boolean show) {
        MengineAppLovinBanner banner = m_banners.get(adUnitId);

        if (banner == null) {
            this.assertionError("not found banner: %s", adUnitId);

            return false;
        }

        if (banner.bannerVisible(show) == false) {
            return false;
        }

        return true;
    }

    public void initInterstitial(String adUnitId) {
        if (m_interstitials.containsKey(adUnitId) == true) {
            this.assertionError("already exist interstitial: %s", adUnitId);
        }

        MengineAppLovinInterstitial interstitial = new MengineAppLovinInterstitial(this, adUnitId);

        m_interstitials.put(adUnitId, interstitial);
    }

    public boolean canYouShowInterstitial(String adUnitId, String placement) {
        MengineAppLovinInterstitial interstitial = m_interstitials.get(adUnitId);

        if (interstitial == null) {
            this.assertionError("not found interstitial: %s", adUnitId);

            return false;
        }

        if (interstitial.canYouShowInterstitial(placement) == false) {
            return false;
        }

        return true;
    }

    public boolean showInterstitial(String adUnitId, String placement) {
        MengineAppLovinInterstitial interstitial = m_interstitials.get(adUnitId);

        if (interstitial == null) {
            this.assertionError("not found interstitial: %s", adUnitId);

            return false;
        }

        if (interstitial.showInterstitial(placement) == false) {
            return false;
        }

        return true;
    }

    public void initRewarded(String adUnitId) {
        if (m_rewardeds.containsKey(adUnitId) == true) {
            this.assertionError("already init rewarded: %s", adUnitId);
        }

        MengineAppLovinRewarded rewarded = new MengineAppLovinRewarded(this, adUnitId);

        m_rewardeds.put(adUnitId, rewarded);
    }

    public boolean canOfferRewarded(String adUnitId, String placement) {
        MengineAppLovinRewarded rewarded = m_rewardeds.get(adUnitId);

        if (rewarded == null) {
            this.assertionError("not found rewarded: %s", adUnitId);

            return false;
        }

        if (rewarded.canOfferRewarded(placement) == false) {
            return false;
        }

        return true;
    }

    public boolean canYouShowRewarded(String adUnitId, String placement) {
        MengineAppLovinRewarded rewarded = m_rewardeds.get(adUnitId);

        if (rewarded == null) {
            this.assertionError("not found rewarded: %s", adUnitId);

            return false;
        }

        if (rewarded.canYouShowRewarded(placement) == false) {
            return false;
        }

        return true;
    }

    public boolean showRewarded(String adUnitId, String placement) {
        MengineAppLovinRewarded rewarded = m_rewardeds.get(adUnitId);

        if (rewarded == null) {
            this.assertionError("not found rewarded: %s", adUnitId);

            return false;
        }

        if (rewarded.showRewarded(placement) == false) {
            return false;
        }

        return true;
    }

    private static MengineAdFormat getAdFormat(MaxAdFormat format) {
        if (format == MaxAdFormat.BANNER) {
            return MengineAdFormat.ADFORMAT_BANNER;
        } else if (format == MaxAdFormat.MREC) {
            return MengineAdFormat.ADFORMAT_MREC;
        } else if (format == MaxAdFormat.LEADER) {
            return MengineAdFormat.ADFORMAT_LEADER;
        } else if (format == MaxAdFormat.INTERSTITIAL) {
            return MengineAdFormat.ADFORMAT_INTERSTITIAL;
        } else if (format == MaxAdFormat.APP_OPEN) {
            return MengineAdFormat.ADFORMAT_APP_OPEN;
        } else if (format == MaxAdFormat.REWARDED) {
            return MengineAdFormat.ADFORMAT_REWARDED;
        } else if (format == MaxAdFormat.REWARDED_INTERSTITIAL) {
            return MengineAdFormat.ADFORMAT_REWARDED_INTERSTITIAL;
        } else if (format == MaxAdFormat.NATIVE) {
            return MengineAdFormat.ADFORMAT_NATIVE;
        } else if (format == MaxAdFormat.CROSS_PROMO) {
            return MengineAdFormat.ADFORMAT_CROSS_PROMO;
        }

        return MengineAdFormat.ADFORMAT_UNKNOWN;
    }

    public void onEventRevenuePaid(MaxAd ad) {
        String networkName = ad.getNetworkName();
        MaxAdFormat format = ad.getFormat();
        MengineAdFormat adFormat = MengineAppLovinPlugin.getAdFormat(format);
        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();
        double revenueValue = ad.getRevenue();
        String revenuePrecision = ad.getRevenuePrecision();

        MengineApplication application = this.getMengineApplication();

        Map<MengineAdRevenueParam, Object> paid = new HashMap<>();
        paid.put(MengineAdRevenueParam.ADREVENUE_MEDIATION, MengineAdMediation.ADMEDIATION_APPLOVINMAX);
        paid.put(MengineAdRevenueParam.ADREVENUE_FORMAT, adFormat);
        paid.put(MengineAdRevenueParam.ADREVENUE_ADUNITID, adUnitId);
        paid.put(MengineAdRevenueParam.ADREVENUE_PLACEMENT, placement);
        paid.put(MengineAdRevenueParam.ADREVENUE_NETWORK, networkName);
        paid.put(MengineAdRevenueParam.ADREVENUE_REVENUE_PRECISION, revenuePrecision);
        paid.put(MengineAdRevenueParam.ADREVENUE_REVENUE_VALUE, revenueValue);
        paid.put(MengineAdRevenueParam.ADREVENUE_REVENUE_CURRENCY, "USD");

        application.onMengineAdRevenue(paid);
    }

    public void showCreativeDebugger() {
        MengineActivity activity = this.getMengineActivity();

        final Context context = activity.getBaseContext();

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);
        appLovinSdk.showCreativeDebugger();
    }

    public void showMediationDebugger() {
        MengineActivity activity = this.getMengineActivity();

        final Context context = activity.getBaseContext();

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);
        appLovinSdk.showMediationDebugger();
    }
}
