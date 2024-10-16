package org.Mengine.Plugin.AppLovin;

import android.graphics.Rect;
import android.net.Uri;
import android.os.Bundle;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxMediatedNetworkInfo;
import com.applovin.sdk.AppLovinCmpError;
import com.applovin.sdk.AppLovinCmpService;
import com.applovin.sdk.AppLovinMediationProvider;
import com.applovin.sdk.AppLovinPrivacySettings;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;
import com.applovin.sdk.AppLovinSdkInitializationConfiguration;
import com.applovin.sdk.AppLovinSdkSettings;
import com.applovin.sdk.AppLovinTermsAndPrivacyPolicyFlowSettings;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdRevenueParam;
import org.Mengine.Base.MengineTransparencyConsentParam;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginEngineListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MengineAppLovinPlugin extends MenginePlugin implements MenginePluginActivityListener, MenginePluginEngineListener {
    public static final String PLUGIN_NAME = "MengineAppLovin";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_SDK_KEY = "mengine.applovin.sdk_key";
    public static final String PLUGIN_METADATA_CCPA = "mengine.applovin.CCPA";
    public static final String PLUGIN_METADATA_ENABLE_PRIVACY_POLICY_FLOW = "mengine.applovin.enable_privacy_policy_flow";
    public static final String PLUGIN_METADATA_PRIVACY_POLICY_URL = "mengine.privacy.privacy_policy_url";
    public static final String PLUGIN_METADATA_TERMS_OF_SERVICE_URL = "mengine.privacy.terms_of_service_url";

    private AppLovinSdk m_appLovinSdk;

    private Map<String, MengineAppLovinBanner> m_banners;
    private Map<String, MengineAppLovinInterstitial> m_interstitials;
    private Map<String, MengineAppLovinRewarded> m_rewardeds;

    private MengineAppLovinMediationInterface m_mediationAmazon;

    private MengineAppLovinNonetBannersInterface m_nonetBanners;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        m_banners = new HashMap<>();
        m_interstitials = new HashMap<>();
        m_rewardeds = new HashMap<>();

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(activity);
        AppLovinSdkSettings settings = appLovinSdk.getSettings();

        String MengineAppLovinPlugin_CCPA = this.getMetaDataString(PLUGIN_METADATA_CCPA);

        this.logMessage("%s: %s"
            , PLUGIN_METADATA_CCPA
            , MengineAppLovinPlugin_CCPA
        );

        if (MengineAppLovinPlugin_CCPA.equalsIgnoreCase("YES") == true) {
            AppLovinPrivacySettings.setDoNotSell(true, activity);
        } else if (MengineAppLovinPlugin_CCPA.equalsIgnoreCase("NO") == true) {
            AppLovinPrivacySettings.setDoNotSell(false, activity);
        } else if (MengineAppLovinPlugin_CCPA.equalsIgnoreCase("UNKNOWN") == true) {
            // Nothing
        } else {
            this.invalidInitialize("invalid %s: %s [YES|NO|UNKNOWN]"
                , PLUGIN_METADATA_CCPA
                , MengineAppLovinPlugin_CCPA
            );
        }

        boolean MengineAppLovinPlugin_EnablePrivacyPolicyFlow = this.getMetaDataBoolean(PLUGIN_METADATA_ENABLE_PRIVACY_POLICY_FLOW);

        if (MengineAppLovinPlugin_EnablePrivacyPolicyFlow == true) {
            AppLovinTermsAndPrivacyPolicyFlowSettings termsAndPrivacyPolicyFlowSettings = settings.getTermsAndPrivacyPolicyFlowSettings();

            termsAndPrivacyPolicyFlowSettings.setEnabled(true);

            String MengineAppLovinPlugin_PrivacyPolicyUrl = this.getMetaDataString(PLUGIN_METADATA_PRIVACY_POLICY_URL);
            termsAndPrivacyPolicyFlowSettings.setPrivacyPolicyUri(Uri.parse(MengineAppLovinPlugin_PrivacyPolicyUrl));

            String MengineAppLovinPlugin_TermsOfServiceUrl = this.getMetaDataString(PLUGIN_METADATA_TERMS_OF_SERVICE_URL);
            termsAndPrivacyPolicyFlowSettings.setTermsOfServiceUri(Uri.parse(MengineAppLovinPlugin_TermsOfServiceUrl));

            this.logMessage("privacy policy: %s"
                , MengineAppLovinPlugin_PrivacyPolicyUrl
            );

            this.logMessage("term of service: %s"
                , MengineAppLovinPlugin_TermsOfServiceUrl
            );

            if (this.hasOption("applovin.consentflow.user_geography.gdpr") == true) {
                this.logMessage("[DEBUG] set debug user geography: GDPR");

                termsAndPrivacyPolicyFlowSettings.setDebugUserGeography(AppLovinSdkConfiguration.ConsentFlowUserGeography.GDPR);
            } else if (this.hasOption("applovin.consentflow.user_geography.other") == true) {
                this.logMessage("[DEBUG] set debug user geography: OTHER");

                termsAndPrivacyPolicyFlowSettings.setDebugUserGeography(AppLovinSdkConfiguration.ConsentFlowUserGeography.OTHER);
            }
        }

        if (BuildConfig.DEBUG == true) {
            settings.setCreativeDebuggerEnabled(true);
        } else {
            settings.setCreativeDebuggerEnabled(false);
        }

        MengineApplication application = this.getMengineApplication();

        String sessionId = application.getSessionId();
        settings.setUserIdentifier(sessionId);

        String MengineAppLovinPlugin_SdkKey = this.getMetaDataString(PLUGIN_METADATA_SDK_KEY);

        this.logMessage("%s: %s"
            , PLUGIN_METADATA_SDK_KEY
            , MengineAppLovinPlugin_SdkKey
        );

        AppLovinSdkInitializationConfiguration config = AppLovinSdkInitializationConfiguration.builder(MengineAppLovinPlugin_SdkKey, activity)
            .setMediationProvider( AppLovinMediationProvider.MAX )
            .build();

        appLovinSdk.initialize(config, new AppLovinSdk.SdkInitializationListener() {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration) {
                AppLovinCmpService cmpService = appLovinSdk.getCmpService();
                boolean supportedCmp = cmpService.hasSupportedCmp();
                boolean testModeEnabled = configuration.isTestModeEnabled();
                String countryCode = configuration.getCountryCode();
                List<String> enabledAmazonAdUnitIds = configuration.getEnabledAmazonAdUnitIds();
                AppLovinSdkConfiguration.ConsentFlowUserGeography consentFlowUserGeography = configuration.getConsentFlowUserGeography();

                MengineAppLovinPlugin.this.logMessage("initialized CMP: %b TestMode: %b CountryCode: %s AmazonAdUnitIds: %s ConsentFlowUserGeography: %s"
                    , supportedCmp
                    , testModeEnabled
                    , countryCode
                    , enabledAmazonAdUnitIds
                    , consentFlowUserGeography.toString()
                );

                List<MaxMediatedNetworkInfo> availableMediatedNetworks = appLovinSdk.getAvailableMediatedNetworks();

                for (MaxMediatedNetworkInfo networkInfo : availableMediatedNetworks) {
                    String name = networkInfo.getName();
                    String adapterVersion = networkInfo.getAdapterVersion();

                    MengineAppLovinPlugin.this.logMessage("available mediated network: %s [%s]"
                        , name
                        , adapterVersion
                    );
                }

                MengineTransparencyConsentParam consent = activity.makeTransparencyConsentParam();

                application.onMengineTransparencyConsent(consent);

                if (MengineAppLovinPlugin.this.hasOption("applovin.show_mediation_debugger") == true) {
                    MengineAppLovinPlugin.this.showMediationDebugger();
                }

                MengineAppLovinPlugin.this.activateSemaphore("AppLovinSdkInitialized");
            }
        });

        m_appLovinSdk = appLovinSdk;

        MengineAppLovinMediationInterface mediationAmazon = this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinMediationAmazon", false);

        if (mediationAmazon != null) {
            mediationAmazon.initializeMediator(activity, this);

            m_mediationAmazon = mediationAmazon;
        } else {
            this.logMessage("not found mediation amazon");
        }

        MengineAppLovinNonetBannersInterface nonetBanners = this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinNonetBanners", false);

        if (nonetBanners != null) {
            nonetBanners.initializeNonetBanners(activity, this);

            m_nonetBanners = nonetBanners;
        } else {
            this.logMessage("not found no-net banners");
        }
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        if (m_mediationAmazon != null) {
            m_mediationAmazon.finalizeMediator(activity, this);
            m_mediationAmazon = null;
        }

        if (m_banners != null) {
            for (MengineAppLovinBanner banner : m_banners.values()) {
                banner.destroy();
            }

            m_banners = null;
        }

        if (m_interstitials != null) {
            for (MengineAppLovinInterstitial interstitial : m_interstitials.values()) {
                interstitial.destroy();
            }

            m_interstitials = null;
        }

        if (m_rewardeds != null) {
            for (MengineAppLovinRewarded rewarded : m_rewardeds.values()) {
                rewarded.destroy();
            }

            m_rewardeds = null;
        }

        if (m_nonetBanners != null) {
            m_nonetBanners.finalizeNonetBanners(activity, this);
            m_nonetBanners = null;
        }
    }

    public MengineAppLovinMediationInterface getMediationAmazon() {
        return m_mediationAmazon;
    }

    public MengineAppLovinNonetBannersInterface getNonetBanners() {
        return m_nonetBanners;
    }

    public boolean initBanner(String adUnitId, String placement) {
        if (m_banners.containsKey(adUnitId) == true) {
            this.assertionError("already exist banner: %s", adUnitId);

            return false;
        }

        this.logMessage("initBanner adUnitId: %s placement: %s"
            , adUnitId
            , placement
        );

        try {
            MengineAppLovinBanner banner = new MengineAppLovinBanner(this, adUnitId, placement);

            m_banners.put(adUnitId, banner);
        } catch (final Exception e) {
            this.logError("[ERROR] initBanner adUnitId: %s placement: %s catch exception: %s"
                , adUnitId
                , placement
                , e.getMessage()
            );

            return false;
        }

        return true;
    }

    public boolean bannerVisible(String adUnitId, boolean show) {
        MengineAppLovinBanner banner = m_banners.get(adUnitId);

        if (banner == null) {
            this.assertionError("not found banner: %s", adUnitId);

            return false;
        }

        this.logMessage("bannerVisible adUnitId: %s show: %b"
            , adUnitId
            , show
        );

        if (banner.bannerVisible(show) == false) {
            return false;
        }

        return true;
    }

    public Rect getBannerViewport(String adUnitId) {
        MengineAppLovinBanner banner = m_banners.get(adUnitId);

        if (banner == null) {
            this.assertionError("not found banner: %s", adUnitId);

            return null;
        }

        Rect rect = banner.getViewport();

        return rect;
    }

    public boolean initInterstitial(String adUnitId) {
        if (m_interstitials.containsKey(adUnitId) == true) {
            this.assertionError("already exist interstitial: %s", adUnitId);

            return false;
        }

        this.logMessage("initInterstitial adUnitId: %s"
            , adUnitId
        );

        try {
            MengineAppLovinInterstitial interstitial = new MengineAppLovinInterstitial(this, adUnitId);

            m_interstitials.put(adUnitId, interstitial);
        } catch (final Exception e) {
            this.logError("[ERROR] initInterstitial adUnitId: %s catch exception: %s"
                , adUnitId
                , e.getMessage()
            );

            return false;
        }

        return true;
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

        this.logMessage("showInterstitial adUnitId: %s placement: %s"
            , adUnitId
            , placement
        );

        MengineActivity activity = this.getMengineActivity();

        if (interstitial.showInterstitial(activity, placement) == false) {
            return false;
        }

        return true;
    }

    public boolean initRewarded(String adUnitId) {
        if (m_rewardeds.containsKey(adUnitId) == true) {
            this.assertionError("already init rewarded: %s", adUnitId);

            return false;
        }

        this.logMessage("initRewarded adUnitId: %s"
            , adUnitId
        );

        try {
            MengineAppLovinRewarded rewarded = new MengineAppLovinRewarded(this, adUnitId);

            m_rewardeds.put(adUnitId, rewarded);
        } catch (final Exception e) {
            this.logError("[ERROR] initRewarded adUnitId: %s catch exception: %s"
                , adUnitId
                , e.getMessage()
            );

            return false;
        }

        return true;
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

        this.logMessage("showRewarded adUnitId: %s placement: %s"
            , adUnitId
            , placement
        );

        MengineActivity activity = this.getMengineActivity();

        if (rewarded.showRewarded(activity, placement) == false) {
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

        MengineAdRevenueParam revenue = new MengineAdRevenueParam();
        revenue.ADREVENUE_MEDIATION = MengineAdMediation.ADMEDIATION_APPLOVINMAX;
        revenue.ADREVENUE_FORMAT = adFormat;
        revenue.ADREVENUE_ADUNITID = adUnitId;
        revenue.ADREVENUE_PLACEMENT = placement;
        revenue.ADREVENUE_NETWORK = networkName;
        revenue.ADREVENUE_REVENUE_PRECISION = revenuePrecision;
        revenue.ADREVENUE_REVENUE_VALUE = revenueValue;
        revenue.ADREVENUE_REVENUE_CURRENCY = "USD";

        application.onMengineAdRevenue(revenue);
    }

    public boolean isConsentFlowUserGeographyGDPR() {
        AppLovinSdkConfiguration configuration = m_appLovinSdk.getConfiguration();

        AppLovinSdkConfiguration.ConsentFlowUserGeography geography = configuration.getConsentFlowUserGeography();

        if (geography != AppLovinSdkConfiguration.ConsentFlowUserGeography.GDPR) {
            return false;
        }

        return true;
    }

    public boolean hasSupportedCmp() {
        AppLovinCmpService cmpService = m_appLovinSdk.getCmpService();

        boolean supportedCmp = cmpService.hasSupportedCmp();

        return supportedCmp;
    }

    public void showConsentFlow() {
        MengineActivity activity = this.getMengineActivity();

        AppLovinCmpService cmpService = m_appLovinSdk.getCmpService();

        cmpService.showCmpForExistingUser(activity, new AppLovinCmpService.OnCompletedListener() {
            @Override
            public void onCompleted(final AppLovinCmpError error) {
                if (error != null) {
                    MengineAppLovinPlugin.this.logError("Failed to show consent dialog error: %s [%s] cmp: %s [%d]"
                        , error.getMessage()
                        , error.getCode().toString()
                        , error.getCmpMessage()
                        , error.getCmpCode()
                    );

                    MengineAppLovinPlugin.this.pythonCall("onAppLovinConsentFlowError");

                    return;
                }

                MengineAppLovinPlugin.this.logMessage("Consent dialog was shown");

                MengineAppLovinPlugin.this.pythonCall("onAppLovinConsentFlowCompleted");
            }
        } );
    }

    public void showCreativeDebugger() {
        if (m_appLovinSdk == null) {
            return;
        }

        m_appLovinSdk.showCreativeDebugger();
    }

    public void showMediationDebugger() {
        if (m_appLovinSdk == null) {
            return;
        }

        m_appLovinSdk.showMediationDebugger();
    }
}
