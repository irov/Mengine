package org.Mengine.Plugin.AppLovin;

import android.net.Uri;
import android.os.Bundle;

import androidx.annotation.NonNull;

import com.applovin.mediation.MaxMediatedNetworkInfo;
import com.applovin.sdk.AppLovinCmpService;
import com.applovin.sdk.AppLovinMediationProvider;
import com.applovin.sdk.AppLovinPrivacySettings;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;
import com.applovin.sdk.AppLovinSdkInitializationConfiguration;
import com.applovin.sdk.AppLovinSdkSettings;
import com.applovin.sdk.AppLovinTermsAndPrivacyPolicyFlowSettings;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdProviderInterface;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerRemoteConfig;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerEngine;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinAdInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinAppOpenAdInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinBannerAdInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinInterstitialAdInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinMRECAdInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinMediationInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinNativeAdInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinNonetBannersInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinPluginInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinRewardedAdInterface;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class MengineAppLovinPlugin extends MengineService implements MengineAppLovinPluginInterface, MengineAdProviderInterface, MengineListenerApplication, MengineListenerActivity, MengineListenerEngine, MengineListenerRemoteConfig, MengineListenerTransparencyConsent {
    public static final String SERVICE_NAME = "AppLovin";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final String METADATA_SDK_KEY = "mengine.applovin.sdk_key";
    public static final String METADATA_ENABLE_PRIVACY_POLICY_FLOW = "mengine.applovin.enable_privacy_policy_flow";
    public static final String METADATA_PRIVACY_POLICY_URL = "mengine.applovin.privacy_policy_url";
    public static final String METADATA_TERMS_OF_SERVICE_URL = "mengine.applovin.terms_of_service_url";

    private boolean m_enableShowMediationDebugger = false;

    private volatile boolean m_appLovinSdkInitialized = false;

    private String m_countryCode;

    private MengineAppLovinBannerAdInterface m_bannerAd;
    private MengineAppLovinInterstitialAdInterface m_interstitialAd;
    private MengineAppLovinRewardedAdInterface m_rewardedAd;
    private MengineAppLovinAppOpenAdInterface m_appOpenAd;
    private MengineAppLovinMRECAdInterface m_MRECAd;
    private MengineAppLovinNativeAdInterface m_nativeAd;

    private final List<MengineAppLovinAdInterface> m_ads = new ArrayList<>();

    private final List<MengineAppLovinMediationInterface> m_mediations = new ArrayList<>();
    private MengineAppLovinNonetBannersInterface m_nonetBanners;

    public boolean getEnableShowMediationDebugger() {
        synchronized (this) {
            return m_enableShowMediationDebugger;
        }
    }

    @Override
    public String onAppVersion(@NonNull MengineApplication application) {
        String sdkVersion = AppLovinSdk.VERSION;

        return sdkVersion;
    }

    protected AppLovinSdk getAppLovinSdkInstance() {
        MengineApplication application = this.getMengineApplication();

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(application);

        return appLovinSdk;
    }

    @SuppressWarnings("unchecked")
    protected <T extends MengineAppLovinAdInterface> T createAd(@NonNull List<String> adUnitIds, @NonNull String className) throws MengineServiceInvalidInitializeException {
        T ad = (T)this.newInstance(className, true, this);

        if (ad == null) {
            this.invalidInitialize("not found AppLovin extension ad: %s"
                , className
            );
        }

        String adUnitId = ad.getAdUnitId();

        adUnitIds.add(adUnitId);

        return ad;
    }

    protected void createaMediation(@NonNull MengineApplication application, @NonNull String className) throws MengineServiceInvalidInitializeException {
        MengineAppLovinMediationInterface mediation = (MengineAppLovinMediationInterface)this.newInstance(className, true);

        if (mediation == null) {
            this.invalidInitialize("not found AppLovin extension mediation: %s"
                , className
            );
        }

        mediation.onAppCreate(application, this);

        m_mediations.add(mediation);
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        MengineAdService adService = application.getService(MengineAdService.class);

        boolean noAds = adService.getNoAds();

        List<String> adUnitIds = new ArrayList<>();

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_BANNERAD == true && noAds == false) {
            m_bannerAd = this.createAd(adUnitIds, "org.Mengine.Plugin.AppLovin.BannerAd.MengineAppLovinBannerAd");

            m_ads.add(m_bannerAd);
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_INTERSTITIALAD == true && noAds == false) {
            m_interstitialAd = this.createAd(adUnitIds, "org.Mengine.Plugin.AppLovin.InterstitialAd.MengineAppLovinInterstitialAd");

            m_ads.add(m_interstitialAd);
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_REWARDEDAD == true) {
            m_rewardedAd = this.createAd(adUnitIds, "org.Mengine.Plugin.AppLovin.RewardedAd.MengineAppLovinRewardedAd");

            m_ads.add(m_rewardedAd);
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_APPOPENAD == true && noAds == false) {
            m_appOpenAd = this.createAd(adUnitIds, "org.Mengine.Plugin.AppLovin.AppOpenAd.MengineAppLovinAppOpenAd");

            m_ads.add(m_appOpenAd);
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_MRECAD == true && noAds == false) {
            m_MRECAd = this.createAd(adUnitIds, "org.Mengine.Plugin.AppLovin.MRECAd.MengineAppLovinMRECAd");

            m_ads.add(m_MRECAd);
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_NATIVEAD == true && noAds == false) {
            m_nativeAd = this.createAd(adUnitIds, "org.Mengine.Plugin.AppLovin.NativeAd.MengineAppLovinNativeAd");

            m_ads.add(m_nativeAd);
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_MEDIATION_AMAZON == true) {
            this.createaMediation(application, "org.Mengine.Plugin.AppLovin.MediationAmazon.MengineAppLovinMediationAmazon");
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_NONET_BANNERS == true) {
            MengineAppLovinNonetBannersInterface nonetBanners = (MengineAppLovinNonetBannersInterface)this.newInstance("org.Mengine.Plugin.AppLovin.NonetBanners.MengineAppLovinNonetBanners", true);

            if (nonetBanners == null) {
                this.invalidInitialize("not found AppLovin extension no-net banners");
            }

            nonetBanners.onAppCreate(application, this);

            m_nonetBanners = nonetBanners;
        }

        AppLovinSdk appLovinSdk = this.getAppLovinSdkInstance();

        AppLovinSdkSettings settings = appLovinSdk.getSettings();

        boolean MengineAppLovinPlugin_EnablePrivacyPolicyFlow = this.getMetaDataBoolean(METADATA_ENABLE_PRIVACY_POLICY_FLOW);

        if (MengineAppLovinPlugin_EnablePrivacyPolicyFlow == true) {
            AppLovinTermsAndPrivacyPolicyFlowSettings termsAndPrivacyPolicyFlowSettings = settings.getTermsAndPrivacyPolicyFlowSettings();

            termsAndPrivacyPolicyFlowSettings.setEnabled(true);

            String MengineAppLovinPlugin_PrivacyPolicyUrl = this.getMetaDataString(METADATA_PRIVACY_POLICY_URL);
            termsAndPrivacyPolicyFlowSettings.setPrivacyPolicyUri(Uri.parse(MengineAppLovinPlugin_PrivacyPolicyUrl));

            String MengineAppLovinPlugin_TermsOfServiceUrl = this.getMetaDataString(METADATA_TERMS_OF_SERVICE_URL);
            termsAndPrivacyPolicyFlowSettings.setTermsOfServiceUri(Uri.parse(MengineAppLovinPlugin_TermsOfServiceUrl));

            this.logInfo("privacy policy: %s"
                , MengineAppLovinPlugin_PrivacyPolicyUrl
            );

            this.logInfo("term of service: %s"
                , MengineAppLovinPlugin_TermsOfServiceUrl
            );

            if (this.hasOption("applovin.consentflow.user_geography.gdpr") == true) {
                this.logMessage("[DEBUG] set debug user geography: GDPR");

                termsAndPrivacyPolicyFlowSettings.setDebugUserGeography(AppLovinSdkConfiguration.ConsentFlowUserGeography.GDPR);
            } else if (this.hasOption("applovin.consentflow.user_geography.other") == true) {
                this.logMessage("[DEBUG] set debug user geography: OTHER");

                termsAndPrivacyPolicyFlowSettings.setDebugUserGeography(AppLovinSdkConfiguration.ConsentFlowUserGeography.OTHER);
            }
        } else {
            MengineParamTransparencyConsent tcParam = application.makeTransparencyConsentParam();

            boolean CCPADoNotSell = tcParam.isCCPADoNotSell();
            AppLovinPrivacySettings.setDoNotSell(CCPADoNotSell);

            boolean hasConsent = tcParam.getConsentAdPersonalization();
            AppLovinPrivacySettings.setHasUserConsent(hasConsent);
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_CREATIVE_DEBUGGER == true) {
            settings.setCreativeDebuggerEnabled(true);
        } else {
            settings.setCreativeDebuggerEnabled(false);
        }

        String userId = application.getUserId();
        if (userId != null) {
            settings.setUserIdentifier(userId);
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_LOGGING_VERBOSE == true) {
            settings.setVerboseLogging(true);
        } else {
            settings.setVerboseLogging(false);
        }

        adService.setAdProvider(this);

        String MengineAppLovinPlugin_SdkKey = this.getMetaDataString(METADATA_SDK_KEY);

        this.logInfo("%s: %s"
            , METADATA_SDK_KEY
            , MengineUtils.getRedactedValue(MengineAppLovinPlugin_SdkKey)
        );

        AppLovinSdkInitializationConfiguration.Builder builder = AppLovinSdkInitializationConfiguration.builder(MengineAppLovinPlugin_SdkKey);

        builder.setMediationProvider(AppLovinMediationProvider.MAX);
        builder.setAdUnitIds(adUnitIds);

        String engineVersion = application.getEngineVersion();
        builder.setPluginVersion("Mengine-v" + engineVersion);

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_TEST_DEVICE_ADVERTISING_ID != null) {
            List<String> testDeviceAdvertisingIds = List.of(BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_TEST_DEVICE_ADVERTISING_ID);

            builder.setTestDeviceAdvertisingIds(testDeviceAdvertisingIds);
        }

        AppLovinSdkInitializationConfiguration config = builder.build();

        this.logInfo("initializing AppLovin SDK with config: %s", config.toString());

        appLovinSdk.initialize(config, configuration -> {
            AppLovinCmpService cmpService = appLovinSdk.getCmpService();
            boolean supportedCmp = cmpService.hasSupportedCmp();
            boolean testModeEnabled = configuration.isTestModeEnabled();
            String countryCode = configuration.getCountryCode();
            List<String> enabledAmazonAdUnitIds = configuration.getEnabledAmazonAdUnitIds();
            AppLovinSdkConfiguration.ConsentFlowUserGeography consentFlowUserGeography = configuration.getConsentFlowUserGeography();

            this.logInfo("initialized AppLovin SDK with CMP: %b TestMode: %b CountryCode: %s AmazonAdUnitIds: %s ConsentFlowUserGeography: %s"
                , supportedCmp
                , testModeEnabled
                , countryCode
                , enabledAmazonAdUnitIds
                , consentFlowUserGeography.toString()
            );

            m_countryCode = countryCode;

            List<MaxMediatedNetworkInfo> availableMediatedNetworks = appLovinSdk.getAvailableMediatedNetworks();

            for (MaxMediatedNetworkInfo networkInfo : availableMediatedNetworks) {
                String name = networkInfo.getName();
                String adapterVersion = networkInfo.getAdapterVersion();

                this.logInfo("available mediated network: %s adapter version: %s"
                    , name
                    , adapterVersion
                );
            }

            application.checkTransparencyConsentServices();

            MengineActivity activity = this.getMengineActivity();

            if (activity != null) {
                if (m_nonetBanners != null) {
                    m_nonetBanners.onActivityCreate(activity);
                }

                for (MengineAppLovinAdInterface ad : m_ads) {
                    ad.onActivityCreate(activity);
                }
            }

            m_appLovinSdkInitialized = true;

            boolean enableShowMediationDebugger = this.getEnableShowMediationDebugger();

            if (enableShowMediationDebugger == true || BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_SHOW_MEDIATION_DEBUGGER == true) {
                this.showMediationDebugger();
            }

            adService.readyAdProvider();
        });
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        m_bannerAd = null;
        m_interstitialAd = null;
        m_rewardedAd = null;
        m_appOpenAd = null;
        m_MRECAd = null;
        m_nativeAd = null;

        m_ads.clear();

        if (m_nonetBanners != null) {
            m_nonetBanners.onAppTerminate(application, this);
            m_nonetBanners = null;
        }

        for (MengineAppLovinMediationInterface mediation : m_mediations) {
            mediation.onAppTerminate(application, this);
        }

        m_mediations.clear();
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        if (m_appLovinSdkInitialized == true) {
            if (m_nonetBanners != null) {
                m_nonetBanners.onActivityCreate(activity);
            }

            for (MengineAppLovinAdInterface ad : m_ads) {
                ad.onActivityCreate(activity);
            }
        }
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        if (m_appLovinSdkInitialized == true) {
            if (m_nonetBanners != null) {
                m_nonetBanners.onActivityDestroy(activity);
            }

            for (MengineAppLovinAdInterface ad : m_ads) {
                ad.onActivityDestroy(activity);
            }
        }
    }

    @Override
    public List<MengineAppLovinMediationInterface> getMediations() {
        return m_mediations;
    }

    @Override
    public MengineAppLovinNonetBannersInterface getNonetBanners() {
        return m_nonetBanners;
    }

    @Override
    public void onMengineRemoteConfigFetch(@NonNull MengineApplication application, @NonNull Map<String, JSONObject> configs) {
        synchronized (this) {
            JSONObject applovin_show_mediation_debugger = configs.getOrDefault("applovin_show_mediation_debugger", null);

            if (applovin_show_mediation_debugger != null) {
                boolean enable = applovin_show_mediation_debugger.optBoolean("enable", false);

                m_enableShowMediationDebugger = enable;
            }

            for (MengineAppLovinMediationInterface mediation : m_mediations) {
                mediation.onMengineRemoteConfigFetch(application, configs);
            }

            if (m_nonetBanners != null) {
                m_nonetBanners.onMengineRemoteConfigFetch(application, configs);
            }
        }
    }

    @Override
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, @NonNull MengineParamTransparencyConsent tcParam) {
        AppLovinSdk appLovinSdk = this.getAppLovinSdkInstance();

        AppLovinSdkSettings settings = appLovinSdk.getSettings();

        AppLovinTermsAndPrivacyPolicyFlowSettings termsAndPrivacyPolicyFlowSettings = settings.getTermsAndPrivacyPolicyFlowSettings();

        if (termsAndPrivacyPolicyFlowSettings.isEnabled() == true) {
            return;
        }

        boolean CCPADoNotSell = tcParam.isCCPADoNotSell();
        AppLovinPrivacySettings.setDoNotSell(CCPADoNotSell);

        boolean hasConsent = tcParam.getConsentAdPersonalization();
        AppLovinPrivacySettings.setHasUserConsent(hasConsent);
    }

    @Override
    public boolean hasBanner() {
        if (m_bannerAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowBanner() {
        if (m_bannerAd == null) {
            return false;
        }

        if (m_bannerAd.canYouShow() == false) {
            return false;
        }

        return true;
    }

    @Override
    public void showBanner() {
        if (m_bannerAd == null) {
            this.assertionError("not found banner");

            return;
        }

        this.logInfo("banner show");

        m_bannerAd.show();
    }

    @Override
    public void hideBanner() {
        if (m_bannerAd == null) {
            this.assertionError("not found banner");

            return;
        }

        this.logInfo("banner hide");

        m_bannerAd.hide();
    }

    @Override
    public int getBannerWidth() {
        if (m_bannerAd == null) {
            this.assertionError("not found banner");

            return 0;
        }

        int widthPx = m_bannerAd.getWidthPx();

        return widthPx;
    }

    @Override
    public int getBannerHeight() {
        if (m_bannerAd == null) {
            this.assertionError("not found banner");

            return 0;
        }

        int heightPx = m_bannerAd.getHeightPx();

        return heightPx;
    }

    @Override
    public boolean hasInterstitial() {
        if (m_interstitialAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowInterstitial(String placement) {
        if (m_interstitialAd == null) {
            return false;
        }

        if (m_interstitialAd.canYouShowInterstitial(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean showInterstitial(String placement) {
        if (m_interstitialAd == null) {
            this.assertionError("invalid show unavailable interstitial placement: %s"
                , placement
            );

            return false;
        }

        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.assertionError("invalid show interstitial activity is null");

            return false;
        }

        this.logInfo("showInterstitial placement: %s"
            , placement
        );

        if (m_interstitialAd.showInterstitial(activity, placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean hasRewarded() {
        if (m_rewardedAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canOfferRewarded(String placement) {
        if (m_rewardedAd == null) {
            return false;
        }

        if (m_rewardedAd.canOfferRewarded(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowRewarded(String placement) {
        if (m_rewardedAd == null) {
            return false;
        }

        if (m_rewardedAd.canYouShowRewarded(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean showRewarded(String placement) {
        if (m_rewardedAd == null) {
            this.assertionError("invalid show unavailable rewarded placement: %s"
                , placement
            );

            return false;
        }

        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.assertionError("invalid show rewarded activity is null");

            return false;
        }

        this.logInfo("showRewarded placement: %s"
            , placement
        );

        if (m_rewardedAd.showRewarded(activity, placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean hasAppOpen() {
        if (m_appOpenAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowAppOpen(String placement, long timeStop) {
        if (m_appOpenAd == null) {
            return false;
        }

        if (m_appOpenAd.canYouShowAppOpen(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean showAppOpen(String placement) {
        if (m_appOpenAd == null) {
            this.assertionError("invalid show unavailable appopen placement: %s"
                , placement
            );

            return false;
        }

        this.logInfo("showAppOpen placement: %s"
            , placement
        );

        if (m_appOpenAd.showAppOpen(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean hasMREC() {
        if (m_MRECAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowMREC() {
        if (m_MRECAd == null) {
            return false;
        }

        if (m_MRECAd.canYouShow() == false) {
            return false;
        }

        return true;
    }

    @Override
    public void showMREC(int leftMargin, int topMargin) {
        if (m_MRECAd == null) {
            this.assertionError("not found MREC");

            return;
        }

        this.logInfo("MREC show");

        m_MRECAd.show(leftMargin, topMargin);
    }

    @Override
    public void hideMREC() {
        if (m_MRECAd == null) {
            this.assertionError("not found MREC");

            return;
        }

        this.logInfo("MREC hide");

        m_MRECAd.hide();
    }

    @Override
    public int getMRECLeftMargin() {
        if (m_MRECAd == null) {
            this.assertionError("not found MREC");

            return 0;
        }

        int leftMarginPx = m_MRECAd.getLeftMarginPx();

        return leftMarginPx;
    }

    @Override
    public int getMRECTopMargin() {
        if (m_MRECAd == null) {
            this.assertionError("not found MREC");

            return 0;
        }

        int topMarginPx = m_MRECAd.getTopMarginPx();

        return topMarginPx;
    }

    @Override
    public int getMRECWidth() {
        if (m_MRECAd == null) {
            this.assertionError("not found MREC");

            return 0;
        }

        int widthPx = m_MRECAd.getWidthPx();

        return widthPx;
    }

    @Override
    public int getMRECHeight() {
        if (m_MRECAd == null) {
            this.assertionError("not found MREC");

            return 0;
        }

        int heightPx = m_MRECAd.getHeightPx();

        return heightPx;
    }

    @Override
    public boolean hasNative() {
        if (m_nativeAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowNative() {
        if (m_nativeAd == null) {
            return false;
        }

        if (m_nativeAd.canYouShow() == false) {
            return false;
        }

        return true;
    }

    @Override
    public void showNative() {
        if (m_nativeAd == null) {
            this.assertionError("not found native");

            return;
        }

        this.logInfo("native show");

        m_nativeAd.show();
    }

    @Override
    public void hideNative() {
        if (m_nativeAd == null) {
            this.assertionError("not found native");

            return;
        }

        this.logInfo("native hide");

        m_nativeAd.hide();
    }

    @Override
    public int getNativeLeftMargin() {
        if (m_nativeAd == null) {
            this.assertionError("not found native");

            return 0;
        }

        int leftMarginPx = m_nativeAd.getLeftMarginPx();

        return leftMarginPx;
    }

    @Override
    public int getNativeTopMargin() {
        if (m_nativeAd == null) {
            this.assertionError("not found native");

            return 0;
        }

        int topMarginPx = m_nativeAd.getTopMarginPx();

        return topMarginPx;
    }

    @Override
    public int getNativeWidth() {
        if (m_nativeAd == null) {
            this.assertionError("not found native");

            return 0;
        }

        int widthPx = m_nativeAd.getWidthPx();

        return widthPx;
    }

    @Override
    public int getNativeHeight() {
        if (m_nativeAd == null) {
            this.assertionError("not found native");

            return 0;
        }

        int heightPx = m_nativeAd.getHeightPx();

        return heightPx;
    }

    public boolean isConsentFlowUserGeographyGDPR() {
        AppLovinSdk appLovinSdk = this.getAppLovinSdkInstance();

        AppLovinSdkConfiguration configuration = appLovinSdk.getConfiguration();

        AppLovinSdkConfiguration.ConsentFlowUserGeography geography = configuration.getConsentFlowUserGeography();

        if (geography != AppLovinSdkConfiguration.ConsentFlowUserGeography.GDPR) {
            return false;
        }

        return true;
    }

    public boolean hasSupportedCmp() {
        AppLovinSdk appLovinSdk = this.getAppLovinSdkInstance();

        AppLovinCmpService cmpService = appLovinSdk.getCmpService();

        boolean supportedCmp = cmpService.hasSupportedCmp();

        return supportedCmp;
    }

    public void showConsentFlow() {
        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.assertionError("invalid show consent flow activity is null");

            return;
        }

        AppLovinSdk appLovinSdk = this.getAppLovinSdkInstance();

        AppLovinCmpService cmpService = appLovinSdk.getCmpService();

        cmpService.showCmpForExistingUser(activity, error -> {
            if (error != null) {
                this.logError("Failed to show consent dialog error: %s code: %s cmp message: %s code: %d"
                    , error.getMessage()
                    , error.getCode().toString()
                    , error.getCmpMessage()
                    , error.getCmpCode()
                );

                this.nativeCall("onAndroidAppLovinConsentFlowError");

                return;
            }

            this.logInfo("Consent dialog was shown");

            this.nativeCall("onAndroidAppLovinConsentFlowCompleted");
        });
    }

    public void showCreativeDebugger() {
        AppLovinSdk appLovinSdk = this.getAppLovinSdkInstance();

        appLovinSdk.showCreativeDebugger();
    }

    public void showMediationDebugger() {
        AppLovinSdk appLovinSdk = this.getAppLovinSdkInstance();

        appLovinSdk.showMediationDebugger();
    }
}
