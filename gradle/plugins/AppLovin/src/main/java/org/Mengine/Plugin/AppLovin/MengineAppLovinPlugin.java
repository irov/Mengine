package org.Mengine.Plugin.AppLovin;

import android.net.Uri;
import android.os.Bundle;

import androidx.annotation.BoolRes;
import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

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
import org.Mengine.Base.MengineConsentFlowUserGeography;
import org.Mengine.Base.MengineFragmentAdvertisingId;
import org.Mengine.Base.MengineFragmentRemoteConfig;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerRemoteConfig;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerEngine;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineTransparencyConsentProviderInterface;
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
import java.util.HashMap;
import java.util.Map;

public class MengineAppLovinPlugin extends MengineService implements MengineAppLovinPluginInterface, MengineAdProviderInterface, MengineListenerApplication, MengineListenerActivity, MengineListenerEngine, MengineListenerRemoteConfig, MengineListenerTransparencyConsent, MengineTransparencyConsentProviderInterface {
    public static final String SERVICE_NAME = "AppLovin";
    public static final boolean SERVICE_EMBEDDING = true;

    @Override
    public boolean isTransparencyConsentProvider() {
        return BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_CONSENT_FLOW;
    }

    public static final @StringRes int METADATA_SDK_KEY = R.string.mengine_applovin_sdk_key;
    public static final @BoolRes int METADATA_ENABLE_PRIVACY_POLICY_FLOW = R.bool.mengine_applovin_enable_privacy_policy_flow;
    public static final @StringRes int METADATA_PRIVACY_POLICY_URL = R.string.privacy_policy_url;
    public static final @StringRes int METADATA_TERMS_OF_SERVICE_URL = R.string.terms_of_service_url;

    private volatile boolean m_enableShowMediationDebugger = false;

    private volatile boolean m_appLovinSdkInitialized = false;

    private MengineAppLovinBannerAdInterface m_bannerAd;
    private MengineAppLovinBannerAdInterface m_topperAd;
    private MengineAppLovinInterstitialAdInterface m_interstitialAd;
    private MengineAppLovinRewardedAdInterface m_rewardedAd;
    private MengineAppLovinAppOpenAdInterface m_appOpenAd;
    private MengineAppLovinMRECAdInterface m_MRECAd;
    private MengineAppLovinNativeAdInterface m_nativeAd;

    private final List<MengineAppLovinAdInterface> m_ads = new ArrayList<>();

    private final List<MengineAppLovinMediationInterface> m_mediations = new ArrayList<>();
    private MengineAppLovinNonetBannersInterface m_nonetBanners;

    public boolean getEnableShowMediationDebugger() {
       return m_enableShowMediationDebugger;
    }

    protected AppLovinSdk getAppLovinSdkInstance() {
        MengineApplication application = this.getMengineApplication();

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(application);

        return appLovinSdk;
    }

    @SuppressWarnings("unchecked")
    protected <T extends MengineAppLovinAdInterface> T createAd(@NonNull String className, Object ... args) throws MengineServiceInvalidInitializeException {
        T ad = (T)this.newInstance(className, true, args);

        if (ad == null) {
            this.invalidInitialize("not found AppLovin extension ad: %s"
                , className
            );
        }

        m_ads.add(ad);

        return ad;
    }

    private Map<String, String> getAdUnitIds() {
        JSONObject config = MengineFragmentRemoteConfig.INSTANCE.getRemoteConfig("applovin_ad_units");

        if (config == null) {
            this.logInfo("[AppLovin] no cached Remote Config applovin_ad_units, no ad units for this session");

            return Map.of();
        }

        Map<String, String> adUnitIds = new HashMap<>();
        String[] formats = {"banner", "topper", "interstitial", "rewarded", "appopen", "mrec", "native"};

        for (String format : formats) {
            Object value = config.opt(format);

            if (value == null) {
                continue;
            }

            if ((value instanceof String) == false) {
                this.logError("[AppLovin] Remote Config applovin_ad_units.%s must be a string", format);

                return Map.of();
            }

            String adUnitId = (String)value;

            if (adUnitId.isEmpty() == true) {
                continue;
            }

            adUnitIds.put(format, adUnitId);
        }

        return Map.copyOf(adUnitIds);
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
    public void onAppPost(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        this.logInfo("[AppLovin SDK] version: %s", AppLovinSdk.VERSION);

        MengineAdService adService = application.getService(MengineAdService.class);

        boolean noAds = adService.getNoAds();

        Map<String, String> configuredAdUnitIds = this.getAdUnitIds();

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_BANNERAD == true && noAds == false) {
            String adUnitId = configuredAdUnitIds.get("banner");

            if (adUnitId != null) {
                m_bannerAd = this.createAd("org.Mengine.Plugin.AppLovin.BannerAd.MengineAppLovinBannerAd", adService, this, adUnitId);
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_TOPPERAD == true && noAds == false) {
            String adUnitId = configuredAdUnitIds.get("topper");

            if (adUnitId != null) {
                m_topperAd = this.createAd("org.Mengine.Plugin.AppLovin.BannerAd.MengineAppLovinBannerAd", adService, this, adUnitId, "topper");
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_INTERSTITIALAD == true && noAds == false) {
            String adUnitId = configuredAdUnitIds.get("interstitial");

            if (adUnitId != null) {
                m_interstitialAd = this.createAd("org.Mengine.Plugin.AppLovin.InterstitialAd.MengineAppLovinInterstitialAd", adService, this, adUnitId);
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_REWARDEDAD == true) {
            String adUnitId = configuredAdUnitIds.get("rewarded");

            if (adUnitId != null) {
                m_rewardedAd = this.createAd("org.Mengine.Plugin.AppLovin.RewardedAd.MengineAppLovinRewardedAd", adService, this, adUnitId);
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_APPOPENAD == true && noAds == false) {
            String adUnitId = configuredAdUnitIds.get("appopen");

            if (adUnitId != null) {
                m_appOpenAd = this.createAd("org.Mengine.Plugin.AppLovin.AppOpenAd.MengineAppLovinAppOpenAd", adService, this, adUnitId);
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_MRECAD == true && noAds == false) {
            String adUnitId = configuredAdUnitIds.get("mrec");

            if (adUnitId != null) {
                m_MRECAd = this.createAd("org.Mengine.Plugin.AppLovin.MRECAd.MengineAppLovinMRECAd", adService, this, adUnitId);
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_NATIVEAD == true && noAds == false) {
            String adUnitId = configuredAdUnitIds.get("native");

            if (adUnitId != null) {
                m_nativeAd = this.createAd("org.Mengine.Plugin.AppLovin.NativeAd.MengineAppLovinNativeAd", adService, this, adUnitId);
            }
        }

        List<String> adUnitIds = new ArrayList<>();

        for (MengineAppLovinAdInterface ad : m_ads) {
            adUnitIds.add(ad.getAdUnitId());
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_MEDIATION_AMAZON == true) {
            this.createaMediation(application, "org.Mengine.Plugin.AppLovin.MediationAmazon.MengineAppLovinMediationAmazon");
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_NONET_BANNERS == true && noAds == false) {
            MengineAppLovinNonetBannersInterface nonetBanners = (MengineAppLovinNonetBannersInterface)this.newInstance("org.Mengine.Plugin.AppLovin.NonetBanners.MengineAppLovinNonetBanners", true);

            if (nonetBanners == null) {
                this.invalidInitialize("not found AppLovin extension no-net banners");
            }

            nonetBanners.onAppCreate(application, this);

            m_nonetBanners = nonetBanners;
        }

        AppLovinSdk appLovinSdk = this.getAppLovinSdkInstance();

        AppLovinSdkSettings settings = appLovinSdk.getSettings();

        boolean MengineAppLovinPlugin_EnablePrivacyPolicyFlow = this.getResourceBoolean(METADATA_ENABLE_PRIVACY_POLICY_FLOW);

        if (MengineAppLovinPlugin_EnablePrivacyPolicyFlow == true) {
            AppLovinTermsAndPrivacyPolicyFlowSettings termsAndPrivacyPolicyFlowSettings = settings.getTermsAndPrivacyPolicyFlowSettings();

            termsAndPrivacyPolicyFlowSettings.setEnabled(true);

            String MengineAppLovinPlugin_PrivacyPolicyUrl = this.getResourceString(METADATA_PRIVACY_POLICY_URL);
            termsAndPrivacyPolicyFlowSettings.setPrivacyPolicyUri(Uri.parse(MengineAppLovinPlugin_PrivacyPolicyUrl));

            String MengineAppLovinPlugin_TermsOfServiceUrl = this.getResourceString(METADATA_TERMS_OF_SERVICE_URL);
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

        String MengineAppLovinPlugin_SdkKey = this.getResourceString(METADATA_SDK_KEY);

        this.logInfo("%s: %s"
            , this.getResourceName(METADATA_SDK_KEY)
            , MengineUtils.getRedactedValue(MengineAppLovinPlugin_SdkKey)
        );

        AppLovinSdkInitializationConfiguration.Builder builder = AppLovinSdkInitializationConfiguration.builder(MengineAppLovinPlugin_SdkKey);

        builder.setMediationProvider(AppLovinMediationProvider.MAX);
        builder.setAdUnitIds(adUnitIds);

        String engineVersion = application.getEngineVersion();
        builder.setPluginVersion("Mengine-v" + engineVersion);

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_TEST_DEVICE_ADVERTISING_ID != null) {
            if ("SELF".equals(BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_TEST_DEVICE_ADVERTISING_ID)) {
                boolean limitAdTracking = MengineFragmentAdvertisingId.INSTANCE.isLimitAdTracking();

                if (limitAdTracking == false) {
                    String advertisingId = MengineFragmentAdvertisingId.INSTANCE.getAdvertisingId();

                    List<String> testDeviceAdvertisingIds = List.of(advertisingId);
                    builder.setTestDeviceAdvertisingIds(testDeviceAdvertisingIds);
                } else {
                    this.logWarning("can not set test device advertising id, user limit ad tracking");
                }
            } else {
                List<String> testDeviceAdvertisingIds = List.of(BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_TEST_DEVICE_ADVERTISING_ID);
                builder.setTestDeviceAdvertisingIds(testDeviceAdvertisingIds);
            }
        }

        AppLovinSdkInitializationConfiguration config = builder.build();

        this.logInfo("[AppLovin SDK] initializing with config: %s", config.toString());

        List<MaxMediatedNetworkInfo> availableMediatedNetworks = appLovinSdk.getAvailableMediatedNetworks();

        for (MaxMediatedNetworkInfo networkInfo : availableMediatedNetworks) {
            String name = networkInfo.getName();
            String adapterVersion = networkInfo.getAdapterVersion();

            this.logInfo("[AppLovin SDK] available mediated network: %s adapter version: %s"
                , name
                , adapterVersion
            );
        }

        appLovinSdk.initialize(config, configuration -> {
            boolean testModeEnabled = configuration.isTestModeEnabled();

            this.logInfo("[AppLovin SDK] initialized TestMode: %b", testModeEnabled);

            if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_CONSENT_FLOW == true) {
                AppLovinCmpService cmpService = appLovinSdk.getCmpService();
                boolean supportedCmp = cmpService.hasSupportedCmp();
                String countryCode = configuration.getCountryCode();

                AppLovinSdkConfiguration.ConsentFlowUserGeography consentFlowUserGeography = configuration.getConsentFlowUserGeography();

                this.logInfo("[AppLovin SDK] supportedCmp: %b countryCode: %s consentFlowUserGeography: %s"
                    , supportedCmp
                    , countryCode
                    , consentFlowUserGeography.toString()
                );

                switch (consentFlowUserGeography) {
                    case UNKNOWN:
                        MengineParamTransparencyConsent.setConsentFlowUserGeography(application, MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_Unknown);
                        break;
                    case GDPR:
                        MengineParamTransparencyConsent.setConsentFlowUserGeography(application, MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_EEA);
                        break;
                    case OTHER:
                        MengineParamTransparencyConsent.setConsentFlowUserGeography(application, MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_NonEEA);
                        break;
                }

                application.checkTransparencyConsentServices();
            }

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
        m_topperAd = null;
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
    public void onMengineRemoteConfigFetch(@NonNull MengineApplication application, boolean updated, @NonNull Map<String, JSONObject> configs, @NonNull Map<String, Integer> ids) {
        JSONObject applovin_show_mediation_debugger = configs.getOrDefault("applovin_show_mediation_debugger", null);

        if (applovin_show_mediation_debugger != null) {
            boolean enable = applovin_show_mediation_debugger.optBoolean("enable", false);

            m_enableShowMediationDebugger = enable;
        }

        for (MengineAppLovinMediationInterface mediation : m_mediations) {
            mediation.onMengineRemoteConfigFetch(application, updated, configs, ids);
        }

        if (m_nonetBanners != null) {
            m_nonetBanners.onMengineRemoteConfigFetch(application, updated, configs, ids);
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
        MengineAppLovinBannerAdInterface bannerAd = m_bannerAd;

        if (bannerAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean isBannerLoaded() {
        MengineAppLovinBannerAdInterface bannerAd = m_bannerAd;

        if (bannerAd == null) {
            return false;
        }

        boolean loaded = bannerAd.isLoaded();

        return loaded;
    }

    @Override
    public boolean canYouShowBanner() {
        MengineAppLovinBannerAdInterface bannerAd = m_bannerAd;

        if (bannerAd == null) {
            return false;
        }

        if (bannerAd.canYouShow() == false) {
            return false;
        }

        return true;
    }

    @Override
    public void showBanner() {
        MengineAppLovinBannerAdInterface bannerAd = m_bannerAd;

        if (bannerAd == null) {
            return;
        }

        if (bannerAd.getView() == null) {
            return;
        }

        bannerAd.show();
    }

    @Override
    public void hideBanner() {
        MengineAppLovinBannerAdInterface bannerAd = m_bannerAd;

        if (bannerAd == null) {
            return;
        }

        if (bannerAd.getView() == null) {
            return;
        }

        bannerAd.hide();
    }

    @Override
    public int getBannerWidth() {
        MengineAppLovinBannerAdInterface bannerAd = m_bannerAd;

        if (bannerAd == null) {
            return 0;
        }

        int widthPx = bannerAd.getWidthPx();

        return widthPx;
    }

    @Override
    public int getBannerHeight() {
        MengineAppLovinBannerAdInterface bannerAd = m_bannerAd;

        if (bannerAd == null) {
            return 0;
        }

        int heightPx = bannerAd.getHeightPx();

        return heightPx;
    }

    @Override
    public boolean hasTopper() {
        MengineAppLovinBannerAdInterface topperAd = m_topperAd;

        if (topperAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean isTopperLoaded() {
        MengineAppLovinBannerAdInterface topperAd = m_topperAd;

        if (topperAd == null) {
            return false;
        }

        boolean loaded = topperAd.isLoaded();

        return loaded;
    }

    @Override
    public boolean canYouShowTopper() {
        MengineAppLovinBannerAdInterface topperAd = m_topperAd;

        if (topperAd == null) {
            return false;
        }

        if (topperAd.canYouShow() == false) {
            return false;
        }

        return true;
    }

    @Override
    public void showTopper() {
        MengineAppLovinBannerAdInterface topperAd = m_topperAd;

        if (topperAd == null) {
            return;
        }

        if (topperAd.getView() == null) {
            return;
        }

        topperAd.show();
    }

    @Override
    public void hideTopper() {
        MengineAppLovinBannerAdInterface topperAd = m_topperAd;

        if (topperAd == null) {
            return;
        }

        if (topperAd.getView() == null) {
            return;
        }

        topperAd.hide();
    }

    @Override
    public int getTopperWidth() {
        MengineAppLovinBannerAdInterface topperAd = m_topperAd;

        if (topperAd == null) {
            return 0;
        }

        int widthPx = topperAd.getWidthPx();

        return widthPx;
    }

    @Override
    public int getTopperHeight() {
        MengineAppLovinBannerAdInterface topperAd = m_topperAd;

        if (topperAd == null) {
            return 0;
        }

        int heightPx = topperAd.getHeightPx();

        return heightPx;
    }

    @Override
    public boolean hasInterstitial() {
        MengineAppLovinInterstitialAdInterface interstitialAd = m_interstitialAd;

        if (interstitialAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowInterstitial(String placement) {
        if (this.isShowingFullscreenAd() == true) {
            return false;
        }

        if (m_interstitialAd == null) {
            return false;
        }

        if (m_interstitialAd.canYouShowInterstitial(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public synchronized boolean showInterstitial(String placement) {
        if (this.isShowingFullscreenAd() == true) {
            return false;
        }

        if (m_interstitialAd == null) {
            this.logWarning("invalid show unavailable interstitial placement: %s"
                , placement
            );

            return false;
        }

        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logWarning("invalid show interstitial activity is null");

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
    public boolean isShowingInterstitial() {
        if (m_interstitialAd == null) {
            return false;
        }

        return m_interstitialAd.isShowingInterstitial();
    }

    @Override
    public boolean hasRewarded() {
        MengineAppLovinRewardedAdInterface rewardedAd = m_rewardedAd;

        if (rewardedAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canOfferRewarded(String placement) {
        if (this.isShowingFullscreenAd() == true) {
            return false;
        }

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
        if (this.isShowingFullscreenAd() == true) {
            return false;
        }

        if (m_rewardedAd == null) {
            return false;
        }

        if (m_rewardedAd.canYouShowRewarded(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public synchronized boolean showRewarded(String placement) {
        if (this.isShowingFullscreenAd() == true) {
            return false;
        }

        if (m_rewardedAd == null) {
            this.logWarning("invalid show unavailable rewarded placement: %s"
                , placement
            );

            return false;
        }

        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logWarning("invalid show rewarded activity is null");

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
    public boolean isShowingRewarded() {
        if (m_rewardedAd == null) {
            return false;
        }

        return m_rewardedAd.isShowingRewarded();
    }

    @Override
    public boolean hasRewardedInterstitial() {
        return false;
    }

    @Override
    public boolean canYouShowRewardedInterstitial(String placement) {
        return false;
    }

    @Override
    public boolean showRewardedInterstitial(String placement) {
        return false;
    }

    @Override
    public boolean isShowingRewardedInterstitial() {
        return false;
    }

    private boolean isShowingFullscreenAd() {
        if (this.isShowingInterstitial() == true) {
            return true;
        }

        if (this.isShowingRewarded() == true) {
            return true;
        }

        if (this.isShowingRewardedInterstitial() == true) {
            return true;
        }

        return false;
    }

    @Override
    public boolean hasAppOpen() {
        MengineAppLovinAppOpenAdInterface appOpenAd = m_appOpenAd;

        if (appOpenAd == null) {
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
            this.logWarning("invalid show unavailable appopen placement: %s"
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
        MengineAppLovinMRECAdInterface MRECAd = m_MRECAd;

        if (MRECAd == null) {
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
            this.logWarning("not found MREC");

            return;
        }

        this.logInfo("MREC show");

        m_MRECAd.show(leftMargin, topMargin);
    }

    @Override
    public void hideMREC() {
        if (m_MRECAd == null) {
            this.logWarning("not found MREC");

            return;
        }

        this.logInfo("MREC hide");

        m_MRECAd.hide();
    }

    @Override
    public int getMRECLeftMargin() {
        if (m_MRECAd == null) {
            return 0;
        }

        int leftMarginPx = m_MRECAd.getLeftMarginPx();

        return leftMarginPx;
    }

    @Override
    public int getMRECTopMargin() {
        if (m_MRECAd == null) {
            return 0;
        }

        int topMarginPx = m_MRECAd.getTopMarginPx();

        return topMarginPx;
    }

    @Override
    public int getMRECWidth() {
        if (m_MRECAd == null) {
            return 0;
        }

        int widthPx = m_MRECAd.getWidthPx();

        return widthPx;
    }

    @Override
    public int getMRECHeight() {
        if (m_MRECAd == null) {
            return 0;
        }

        int heightPx = m_MRECAd.getHeightPx();

        return heightPx;
    }

    @Override
    public boolean hasNative() {
        MengineAppLovinNativeAdInterface nativeAd = m_nativeAd;

        if (nativeAd == null) {
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
            this.logWarning("not found native");

            return;
        }

        this.logInfo("native show");

        m_nativeAd.show();
    }

    @Override
    public void hideNative() {
        if (m_nativeAd == null) {
            this.logWarning("not found native");

            return;
        }

        this.logInfo("native hide");

        m_nativeAd.hide();
    }

    @Override
    public int getNativeLeftMargin() {
        if (m_nativeAd == null) {
            return 0;
        }

        int leftMarginPx = m_nativeAd.getLeftMarginPx();

        return leftMarginPx;
    }

    @Override
    public int getNativeTopMargin() {
        if (m_nativeAd == null) {
            return 0;
        }

        int topMarginPx = m_nativeAd.getTopMarginPx();

        return topMarginPx;
    }

    @Override
    public int getNativeWidth() {
        if (m_nativeAd == null) {
            return 0;
        }

        int widthPx = m_nativeAd.getWidthPx();

        return widthPx;
    }

    @Override
    public int getNativeHeight() {
        if (m_nativeAd == null) {
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
            this.logError("invalid show consent flow activity is null");

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
