package org.Mengine.Plugin.AppLovin;

import android.net.Uri;
import android.os.Bundle;

import androidx.annotation.NonNull;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
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
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdProviderInterface;
import org.Mengine.Base.MengineParamAdRevenue;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineFragmentAdRevenue;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerRemoteConfig;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerEngine;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class MengineAppLovinPlugin extends MengineService implements MengineAdProviderInterface, MengineListenerApplication, MengineListenerActivity, MengineListenerEngine, MengineListenerRemoteConfig {
    public static final String SERVICE_NAME = "AppLovin";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final String METADATA_SDK_KEY = "mengine.applovin.sdk_key";
    public static final String METADATA_CCPA = "mengine.applovin.CCPA";
    public static final String METADATA_ENABLE_PRIVACY_POLICY_FLOW = "mengine.applovin.enable_privacy_policy_flow";
    public static final String METADATA_PRIVACY_POLICY_URL = "mengine.applovin.privacy_policy_url";
    public static final String METADATA_TERMS_OF_SERVICE_URL = "mengine.applovin.terms_of_service_url";
    public static final String METADATA_BANNER_PLACEMENT = "mengine.applovin.banner.placement";
    public static final String METADATA_BANNER_ADUNITID = "mengine.applovin.banner.adunitid";
    public static final String METADATA_INTERSTITIAL_ADUNITID = "mengine.applovin.interstitial.adunitid";
    public static final String METADATA_REWARDED_ADUNITID = "mengine.applovin.rewarded.adunitid";
    public static final String METADATA_APPOPEN_PLACEMENT = "mengine.applovin.appopen.placement";
    public static final String METADATA_APPOPEN_ADUNITID = "mengine.applovin.appopen.adunitid";
    public static final String METADATA_MREC_PLACEMENT = "mengine.applovin.mrec.placement";
    public static final String METADATA_MREC_ADUNITID = "mengine.applovin.mrec.adunitid";
    public static final String METADATA_NATIVE_PLACEMENT = "mengine.applovin.native.placement";
    public static final String METADATA_NATIVE_ADUNITID = "mengine.applovin.native.adunitid";

    private boolean m_enableShowMediationDebugger = false;

    private String m_countryCode;

    private MengineAppLovinBannerAd m_bannerAd;
    private MengineAppLovinInterstitialAd m_interstitialAd;
    private MengineAppLovinRewardedAd m_rewardedAd;
    private MengineAppLovinAppOpenAd m_appOpenAd;
    private MengineAppLovinMRECAd m_MRECAd;
    private MengineAppLovinNativeAd m_nativeAd;

    private MengineAppLovinMediationInterface m_mediationAmazon;
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

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        MengineAdService adService = application.getService(MengineAdService.class);

        boolean noAds = adService.getNoAds();

        List<String> adUnitIds = new ArrayList<>();

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_BANNERAD == true && noAds == false) {
            String MengineAppLovinPlugin_Banner_AdUnitId = this.getMetaDataString(METADATA_BANNER_ADUNITID);

            if (MengineAppLovinPlugin_Banner_AdUnitId.isEmpty() == false) {
                adUnitIds.add(MengineAppLovinPlugin_Banner_AdUnitId);

                String MengineAppLovinPlugin_Banner_Placement = this.getMetaDataString(METADATA_BANNER_PLACEMENT);

                MengineAppLovinBannerAd bannerAd = new MengineAppLovinBannerAd(this, MengineAppLovinPlugin_Banner_AdUnitId, MengineAppLovinPlugin_Banner_Placement);

                m_bannerAd = bannerAd;
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_INTERSTITIALAD == true && noAds == false) {
            String MengineAppLovinPlugin_Interstitial_AdUnitId = this.getMetaDataString(METADATA_INTERSTITIAL_ADUNITID);

            if (MengineAppLovinPlugin_Interstitial_AdUnitId.isEmpty() == false) {
                adUnitIds.add(MengineAppLovinPlugin_Interstitial_AdUnitId);

                MengineAppLovinInterstitialAd interstitialAd = new MengineAppLovinInterstitialAd(this, MengineAppLovinPlugin_Interstitial_AdUnitId);

                m_interstitialAd = interstitialAd;
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_REWARDEDAD == true) {
            String MengineAppLovinPlugin_Rewarded_AdUnitId = this.getMetaDataString(METADATA_REWARDED_ADUNITID);

            if (MengineAppLovinPlugin_Rewarded_AdUnitId.isEmpty() == false) {
                adUnitIds.add(MengineAppLovinPlugin_Rewarded_AdUnitId);

                MengineAppLovinRewardedAd rewardedAd = new MengineAppLovinRewardedAd(this, MengineAppLovinPlugin_Rewarded_AdUnitId);

                m_rewardedAd = rewardedAd;
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_APPOPENAD == true && noAds == false) {
            String MengineAppLovinPlugin_AppOpen_AdUnitId = this.getMetaDataString(METADATA_APPOPEN_ADUNITID);

            if (MengineAppLovinPlugin_AppOpen_AdUnitId.isEmpty() == false) {
                adUnitIds.add(MengineAppLovinPlugin_AppOpen_AdUnitId);

                String MengineAppLovinPlugin_AppOpen_Placement = this.getMetaDataString(METADATA_APPOPEN_PLACEMENT);

                MengineAppLovinAppOpenAd appOpenAd = new MengineAppLovinAppOpenAd(this, MengineAppLovinPlugin_AppOpen_AdUnitId, MengineAppLovinPlugin_AppOpen_Placement);

                m_appOpenAd = appOpenAd;
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_MRECAD == true && noAds == false) {
            String MengineAppLovinPlugin_MREC_AdUnitId = this.getMetaDataString(METADATA_MREC_ADUNITID);

            if (MengineAppLovinPlugin_MREC_AdUnitId.isEmpty() == false) {
                adUnitIds.add(MengineAppLovinPlugin_MREC_AdUnitId);

                String MengineAppLovinPlugin_MREC_Placement = this.getMetaDataString(METADATA_MREC_PLACEMENT);

                MengineAppLovinMRECAd mrecAd = new MengineAppLovinMRECAd(this, MengineAppLovinPlugin_MREC_AdUnitId, MengineAppLovinPlugin_MREC_Placement);

                m_MRECAd = mrecAd;
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_NATIVEAD == true && noAds == false) {
            String MengineAppLovinPlugin_Native_AdUnitId = this.getMetaDataString(METADATA_NATIVE_ADUNITID);

            if (MengineAppLovinPlugin_Native_AdUnitId.isEmpty() == false) {
                adUnitIds.add(MengineAppLovinPlugin_Native_AdUnitId);

                String MengineAppLovinPlugin_Native_Placement = this.getMetaDataString(METADATA_NATIVE_PLACEMENT);

                MengineAppLovinNativeAd nativeAd = new MengineAppLovinNativeAd(this, MengineAppLovinPlugin_Native_AdUnitId, MengineAppLovinPlugin_Native_Placement);

                m_nativeAd = nativeAd;
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_MEDIATION_META == true) {
            com.facebook.ads.AdSettings.setDataProcessingOptions( new String[] {} );
        }

        AppLovinSdk appLovinSdk = this.getAppLovinSdkInstance();

        AppLovinSdkSettings settings = appLovinSdk.getSettings();

        String MengineAppLovinPlugin_CCPA = this.getMetaDataString(METADATA_CCPA);

        this.logInfo("%s: %s"
            , METADATA_CCPA
            , MengineAppLovinPlugin_CCPA
        );

        if (MengineAppLovinPlugin_CCPA.equalsIgnoreCase("YES") == true) {
            AppLovinPrivacySettings.setDoNotSell(true);
        } else if (MengineAppLovinPlugin_CCPA.equalsIgnoreCase("NO") == true) {
            AppLovinPrivacySettings.setDoNotSell(false);
        } else if (MengineAppLovinPlugin_CCPA.equalsIgnoreCase("UNKNOWN") == true) {
            // Nothing
        } else {
            this.invalidInitialize("invalid %s: %s [YES|NO|UNKNOWN]"
                , METADATA_CCPA
                , MengineAppLovinPlugin_CCPA
            );
        }

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
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_CREATIVE_DEBUGGER == true) {
            settings.setCreativeDebuggerEnabled(true);
        } else {
            settings.setCreativeDebuggerEnabled(false);
        }

        String userId = application.getUserId();
        settings.setUserIdentifier(userId);

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_LOGGING_VERBOSE == true) {
            settings.setVerboseLogging(true);
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

            this.logInfo("initialized CMP: %b TestMode: %b CountryCode: %s AmazonAdUnitIds: %s ConsentFlowUserGeography: %s"
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

            if (m_bannerAd != null) {
                m_bannerAd.initialize(application);
            }

            if (m_interstitialAd != null) {
                m_interstitialAd.initialize(application);
            }

            if (m_rewardedAd != null) {
                m_rewardedAd.initialize(application);
            }

            if (m_appOpenAd != null) {
                m_appOpenAd.initialize(application);
            }

            if (m_MRECAd != null) {
                m_MRECAd.initialize(application);
            }

            if (m_nativeAd != null) {
                m_nativeAd.initialize(application);
            }

            MengineActivity activity = this.getMengineActivity();

            if (activity != null) {
                if (m_bannerAd != null) {
                    m_bannerAd.onActivityCreate(activity);
                }

                if (m_interstitialAd != null) {
                    m_interstitialAd.onActivityCreate(activity);
                }

                if (m_rewardedAd != null) {
                    m_rewardedAd.onActivityCreate(activity);
                }

                if (m_appOpenAd != null) {
                    m_appOpenAd.onActivityCreate(activity);
                }

                if (m_MRECAd != null) {
                    m_MRECAd.onActivityCreate(activity);
                }

                if (m_nativeAd != null) {
                    m_nativeAd.onActivityCreate(activity);
                }
            }

            boolean enableShowMediationDebugger = this.getEnableShowMediationDebugger();

            if (enableShowMediationDebugger == true || BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_SHOW_MEDIATION_DEBUGGER == true) {
                this.showMediationDebugger();
            }

            adService.readyAdProvider();
        });

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_MEDIATION_AMAZON == true) {
            MengineAppLovinMediationInterface mediationAmazon = (MengineAppLovinMediationInterface)this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinMediationAmazon", false);

            if (mediationAmazon != null) {
                mediationAmazon.onAppCreate(application, this);

                m_mediationAmazon = mediationAmazon;
            } else {
                this.logError("not found AppLovin extension Amazon mediation");
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_APPLOVIN_NONET_BANNERS == true) {
            MengineAppLovinNonetBannersInterface nonetBanners = (MengineAppLovinNonetBannersInterface)this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinNonetBanners", false);

            if (nonetBanners != null) {
                nonetBanners.onAppCreate(application, this);

                m_nonetBanners = nonetBanners;
            } else {
                this.logError("not found AppLovin extension no-net banners");
            }
        }
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        if (m_mediationAmazon != null) {
            m_mediationAmazon.onAppTerminate(application, this);
            m_mediationAmazon = null;
        }

        if (m_bannerAd != null) {
            m_bannerAd.finalize(application);
            m_bannerAd = null;
        }

        if (m_interstitialAd != null) {
            m_interstitialAd.finalize(application);
            m_interstitialAd = null;
        }

        if (m_rewardedAd != null) {
            m_rewardedAd.finalize(application);
            m_rewardedAd = null;
        }

        if (m_appOpenAd != null) {
            m_appOpenAd.finalize(application);
            m_appOpenAd = null;
        }

        if (m_MRECAd != null) {
            m_MRECAd.finalize(application);
            m_MRECAd = null;
        }

        if (m_nativeAd != null) {
            m_nativeAd.finalize(application);
            m_nativeAd = null;
        }

        if (m_nonetBanners != null) {
            m_nonetBanners.onAppTerminate(application, this);
            m_nonetBanners = null;
        }
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        if (m_nonetBanners != null) {
            m_nonetBanners.onActivityCreate(activity);
        }

        if (m_bannerAd != null && m_bannerAd.isInitialized() == true) {
            m_bannerAd.onActivityCreate(activity);
        }

        if (m_interstitialAd != null && m_interstitialAd.isInitialized() == true) {
            m_interstitialAd.onActivityCreate(activity);
        }

        if (m_rewardedAd != null && m_rewardedAd.isInitialized() == true) {
            m_rewardedAd.onActivityCreate(activity);
        }

        if (m_appOpenAd != null && m_appOpenAd.isInitialized() == true) {
            m_appOpenAd.onActivityCreate(activity);
        }

        if (m_MRECAd != null && m_MRECAd.isInitialized() == true) {
            m_MRECAd.onActivityCreate(activity);
        }

        if (m_nativeAd != null && m_nativeAd.isInitialized() == true) {
            m_nativeAd.onActivityCreate(activity);
        }
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        if (m_nonetBanners != null) {
            m_nonetBanners.onActivityDestroy(activity);
        }

        if (m_bannerAd != null && m_bannerAd.isInitialized() == true) {
            m_bannerAd.onActivityDestroy(activity);
        }

        if (m_interstitialAd != null && m_interstitialAd.isInitialized() == true) {
            m_interstitialAd.onActivityDestroy(activity);
        }

        if (m_rewardedAd != null && m_rewardedAd.isInitialized() == true) {
            m_rewardedAd.onActivityDestroy(activity);
        }

        if (m_appOpenAd != null && m_appOpenAd.isInitialized() == true) {
            m_appOpenAd.onActivityDestroy(activity);
        }

        if (m_MRECAd != null && m_MRECAd.isInitialized() == true) {
            m_MRECAd.onActivityDestroy(activity);
        }

        if (m_nativeAd != null && m_nativeAd.isInitialized() == true) {
            m_nativeAd.onActivityDestroy(activity);
        }
    }

    @Override
    public void onMengineRemoteConfigFetch(@NonNull MengineApplication application, @NonNull Map<String, JSONObject> configs) {
        synchronized (this) {
            JSONObject applovin_show_mediation_debugger = configs.getOrDefault("applovin_show_mediation_debugger", null);

            if (applovin_show_mediation_debugger != null) {
                boolean enable = applovin_show_mediation_debugger.optBoolean("enable", false);

                m_enableShowMediationDebugger = enable;
            }
        }
    }

    public MengineAppLovinMediationInterface getMediationAmazon() {
        return m_mediationAmazon;
    }

    public MengineAppLovinNonetBannersInterface getNonetBanners() {
        return m_nonetBanners;
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
            return MengineAdFormat.ADFORMAT_APPOPEN;
        } else if (format == MaxAdFormat.REWARDED) {
            return MengineAdFormat.ADFORMAT_REWARDED;
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

        MengineParamAdRevenue revenue = new MengineParamAdRevenue();
        revenue.ADREVENUE_MEDIATION = MengineAdMediation.ADMEDIATION_APPLOVINMAX;
        revenue.ADREVENUE_FORMAT = adFormat;
        revenue.ADREVENUE_ADUNITID = adUnitId;
        revenue.ADREVENUE_PLACEMENT = placement;
        revenue.ADREVENUE_NETWORK = networkName;
        revenue.ADREVENUE_REVENUE_PRECISION = revenuePrecision;
        revenue.ADREVENUE_REVENUE_VALUE = revenueValue;
        revenue.ADREVENUE_REVENUE_CURRENCY = "USD";

        MengineFragmentAdRevenue.INSTANCE.adRevenue(revenue);
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

                this.pythonCall("onAndroidAppLovinConsentFlowError");

                return;
            }

            this.logInfo("Consent dialog was shown");

            this.pythonCall("onAndroidAppLovinConsentFlowCompleted");
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
