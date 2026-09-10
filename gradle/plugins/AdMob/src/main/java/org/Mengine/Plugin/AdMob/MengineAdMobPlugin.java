package org.Mengine.Plugin.AdMob;

import android.os.Bundle;

import androidx.annotation.NonNull;

import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.VersionInfo;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdProviderInterface;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFragmentRemoteConfig;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobAdInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobBannerAdInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobInterstitialAdInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobPluginInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobRewardedAdInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobRewardedInterstitialAdInterface;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.Map;

public class MengineAdMobPlugin extends MengineService implements MengineAdMobPluginInterface, MengineAdProviderInterface, MengineListenerApplication, MengineListenerActivity, MengineListenerTransparencyConsent {
    public static final String SERVICE_NAME = "AdMob";
    public static final boolean SERVICE_EMBEDDING = true;

    private volatile boolean m_adMobSdkInitialized = false;
    private boolean m_initializationAttempted = false;
    private boolean m_applicationReady = false;

    private volatile MengineAdMobBannerAdInterface m_bannerAd;
    private volatile MengineAdMobBannerAdInterface m_topperAd;
    private volatile MengineAdMobInterstitialAdInterface m_interstitialAd;
    private volatile MengineAdMobRewardedAdInterface m_rewardedAd;
    private volatile MengineAdMobRewardedInterstitialAdInterface m_rewardedInterstitialAd;

    private final List<MengineAdMobAdInterface> m_ads = new ArrayList<>();
    private Map<String, String> m_adUnitIds = Map.of();

    public boolean isSdkInitialized() {
        return m_adMobSdkInitialized;
    }

    @Override
    public MengineAdMobBannerAdInterface getBannerAd() {
        return m_bannerAd;
    }

    @Override
    public MengineAdMobInterstitialAdInterface getInterstitialAd() {
        return m_interstitialAd;
    }

    @Override
    public MengineAdMobRewardedAdInterface getRewardedAd() {
        return m_rewardedAd;
    }

    @SuppressWarnings("unchecked")
    protected <T extends MengineAdMobAdInterface> T createAd(@NonNull String className, Object ... args) throws MengineServiceInvalidInitializeException {
        T ad = (T)this.newInstance(className, true, args);

        if (ad == null) {
            this.invalidInitialize("not found AdMob extension ad: %s"
                , className
            );
        }

        m_ads.add(ad);

        return ad;
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        MengineAdService adService = application.getService(MengineAdService.class);

        adService.setAdProvider(this);
    }

    @Override
    public void onAppPost(@NonNull MengineApplication application) {
        m_applicationReady = true;

        MengineParamTransparencyConsent consent = application.makeTransparencyConsentParam();

        this.onMengineTransparencyConsent(application, consent);
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        m_applicationReady = false;

        MengineActivity activity = this.getMengineActivity();

        if (activity != null) {
            this.destroyAds(activity);
        }

        m_adUnitIds = Map.of();
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) {
        if (m_adMobSdkInitialized == false) {
            return;
        }

        this.initializeAds(activity);
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        this.destroyAds(activity);
    }

    private void initializeAds(@NonNull MengineActivity activity) {
        MengineApplication application = this.getMengineApplication();
        MengineAdService adService = application.getService(MengineAdService.class);

        try {
            this.createAds(adService, m_adUnitIds);

            for (MengineAdMobAdInterface ad : m_ads) {
                ad.onActivityCreate(activity);
            }
        } catch (final MengineServiceInvalidInitializeException | RuntimeException e) {
            this.destroyAds(activity);
            this.logException(e, Map.of());
        }
    }

    private void destroyAds(@NonNull MengineActivity activity) {
        for (MengineAdMobAdInterface ad : m_ads) {
            ad.onActivityDestroy(activity);
        }

        m_ads.clear();

        m_bannerAd = null;
        m_topperAd = null;
        m_interstitialAd = null;
        m_rewardedAd = null;
        m_rewardedInterstitialAd = null;
    }

    @Override
    public boolean hasBanner() {
        MengineAdMobBannerAdInterface bannerAd = m_bannerAd;

        if (bannerAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean isBannerLoaded() {
        MengineAdMobBannerAdInterface bannerAd = m_bannerAd;

        if (bannerAd == null) {
            return false;
        }

        boolean loaded = bannerAd.isLoaded();

        return loaded;
    }

    @Override
    public boolean canYouShowBanner() {
        MengineAdMobBannerAdInterface bannerAd = m_bannerAd;

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
        MengineAdMobBannerAdInterface bannerAd = m_bannerAd;

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
        MengineAdMobBannerAdInterface bannerAd = m_bannerAd;

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
        MengineAdMobBannerAdInterface bannerAd = m_bannerAd;

        if (bannerAd == null) {
            return 0;
        }

        int widthPx = bannerAd.getWidthPx();

        return widthPx;
    }

    @Override
    public int getBannerHeight() {
        MengineAdMobBannerAdInterface bannerAd = m_bannerAd;

        if (bannerAd == null) {
            return 0;
        }

        int heightPx = bannerAd.getHeightPx();

        return heightPx;
    }

    @Override
    public boolean hasTopper() {
        MengineAdMobBannerAdInterface topperAd = m_topperAd;

        if (topperAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean isTopperLoaded() {
        MengineAdMobBannerAdInterface topperAd = m_topperAd;

        if (topperAd == null) {
            return false;
        }

        boolean loaded = topperAd.isLoaded();

        return loaded;
    }

    @Override
    public boolean canYouShowTopper() {
        MengineAdMobBannerAdInterface topperAd = m_topperAd;

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
        MengineAdMobBannerAdInterface topperAd = m_topperAd;

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
        MengineAdMobBannerAdInterface topperAd = m_topperAd;

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
        MengineAdMobBannerAdInterface topperAd = m_topperAd;

        if (topperAd == null) {
            return 0;
        }

        int widthPx = topperAd.getWidthPx();

        return widthPx;
    }

    @Override
    public int getTopperHeight() {
        MengineAdMobBannerAdInterface topperAd = m_topperAd;

        if (topperAd == null) {
            return 0;
        }

        int heightPx = topperAd.getHeightPx();

        return heightPx;
    }

    @Override
    public boolean hasInterstitial() {
        MengineAdMobInterstitialAdInterface interstitialAd = m_interstitialAd;

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

        MengineAdMobInterstitialAdInterface interstitialAd = m_interstitialAd;

        if (interstitialAd == null) {
            return false;
        }

        if (interstitialAd.canYouShowInterstitial(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public synchronized boolean showInterstitial(String placement) {
        if (this.isShowingFullscreenAd() == true) {
            return false;
        }

        MengineAdMobInterstitialAdInterface interstitialAd = m_interstitialAd;

        if (interstitialAd == null) {
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

        if (interstitialAd.showInterstitial(activity, placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean isShowingInterstitial() {
        MengineAdMobInterstitialAdInterface interstitialAd = m_interstitialAd;

        if (interstitialAd == null) {
            return false;
        }

        boolean showing = interstitialAd.isShowingInterstitial();

        return showing;
    }

    @Override
    public boolean hasRewarded() {
        MengineAdMobRewardedAdInterface rewardedAd = m_rewardedAd;

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

        MengineAdMobRewardedAdInterface rewardedAd = m_rewardedAd;

        if (rewardedAd == null) {
            return false;
        }

        if (rewardedAd.canOfferRewarded(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowRewarded(String placement) {
        if (this.isShowingFullscreenAd() == true) {
            return false;
        }

        MengineAdMobRewardedAdInterface rewardedAd = m_rewardedAd;

        if (rewardedAd == null) {
            return false;
        }

        if (rewardedAd.canYouShowRewarded(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public synchronized boolean showRewarded(String placement) {
        if (this.isShowingFullscreenAd() == true) {
            return false;
        }

        MengineAdMobRewardedAdInterface rewardedAd = m_rewardedAd;

        if (rewardedAd == null) {
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

        if (rewardedAd.showRewarded(activity, placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean isShowingRewarded() {
        MengineAdMobRewardedAdInterface rewardedAd = m_rewardedAd;

        if (rewardedAd == null) {
            return false;
        }

        boolean showing = rewardedAd.isShowingRewarded();

        return showing;
    }

    @Override
    public boolean hasRewardedInterstitial() {
        MengineAdMobRewardedInterstitialAdInterface rewardedInterstitialAd = m_rewardedInterstitialAd;

        if (rewardedInterstitialAd == null) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowRewardedInterstitial(String placement) {
        if (this.isShowingFullscreenAd() == true) {
            return false;
        }

        MengineAdMobRewardedInterstitialAdInterface rewardedInterstitialAd = m_rewardedInterstitialAd;

        if (rewardedInterstitialAd == null) {
            return false;
        }

        if (rewardedInterstitialAd.canYouShowRewardedInterstitial(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public synchronized boolean showRewardedInterstitial(String placement) {
        if (this.isShowingFullscreenAd() == true) {
            return false;
        }

        MengineAdMobRewardedInterstitialAdInterface rewardedInterstitialAd = m_rewardedInterstitialAd;

        if (rewardedInterstitialAd == null) {
            this.logWarning("invalid show unavailable rewarded interstitial placement: %s"
                , placement
            );

            return false;
        }

        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logWarning("invalid show rewarded interstitial activity is null");

            return false;
        }

        this.logInfo("showRewardedInterstitial placement: %s"
            , placement
        );

        if (rewardedInterstitialAd.showRewardedInterstitial(activity, placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean isShowingRewardedInterstitial() {
        MengineAdMobRewardedInterstitialAdInterface rewardedInterstitialAd = m_rewardedInterstitialAd;

        if (rewardedInterstitialAd == null) {
            return false;
        }

        boolean showing = rewardedInterstitialAd.isShowingRewardedInterstitial();

        return showing;
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
        //TODO
        return false;
    }

    @Override
    public boolean canYouShowAppOpen(String placement, long timeStop) {
        //TODO
        return false;
    }

    @Override
    public boolean showAppOpen(String placement) {
        //TODO
        return false;
    }

    @Override
    public boolean hasMREC() {
        //TODO
        return false;
    }

    @Override
    public boolean canYouShowMREC() {
        //TODO
        return false;
    }

    @Override
    public void showMREC(int leftMargin, int topMargin) {
        //TODO
    }

    @Override
    public void hideMREC() {
        //TODO
    }

    @Override
    public int getMRECLeftMargin() {
        //TODO
        return 0;
    }

    @Override
    public int getMRECTopMargin() {
        //TODO
        return 0;
    }

    @Override
    public int getMRECWidth() {
        //TODO
        return 0;
    }

    @Override
    public int getMRECHeight() {
        //TODO
        return 0;
    }

    @Override
    public boolean hasNative() {
        //TODO
        return false;
    }

    @Override
    public boolean canYouShowNative() {
        //TODO
        return false;
    }

    @Override
    public void showNative() {
        //TODO
    }

    @Override
    public void hideNative() {
        //TODO
    }

    @Override
    public int getNativeLeftMargin() {
        //TODO
        return 0;
    }

    @Override
    public int getNativeTopMargin() {
        //TODO
        return 0;
    }

    @Override
    public int getNativeWidth() {
        //TODO
        return 0;
    }

    @Override
    public int getNativeHeight() {
        //TODO
        return 0;
    }

    private void createAds(@NonNull MengineAdService adService, @NonNull Map<String, String> adUnitIds) throws MengineServiceInvalidInitializeException {
        boolean noAds = adService.getNoAds();

        if (BuildConfig.MENGINE_APP_PLUGIN_ADMOB_BANNERAD == true) {
            if (noAds == false) {
                String adUnitId = adUnitIds.get("banner");

                if (adUnitId != null) {
                    m_bannerAd = this.createAd("org.Mengine.Plugin.AdMob.BannerAd.MengineAdMobBannerAd", adService, this, adUnitId);
                }
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_ADMOB_BANNERAD == true) {
            if (noAds == false) {
                String adUnitId = adUnitIds.get("topper");

                if (adUnitId != null) {
                    m_topperAd = this.createAd("org.Mengine.Plugin.AdMob.BannerAd.MengineAdMobBannerAd", adService, this, adUnitId, "topper");
                }
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_ADMOB_INTERSTITIALAD == true) {
            if (noAds == false) {
                String adUnitId = adUnitIds.get("interstitial");

                if (adUnitId != null) {
                    m_interstitialAd = this.createAd("org.Mengine.Plugin.AdMob.InterstitialAd.MengineAdMobInterstitialAd", adService, this, adUnitId);
                }
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_ADMOB_REWARDEDAD == true) {
            String adUnitId = adUnitIds.get("rewarded");

            if (adUnitId != null) {
                m_rewardedAd = this.createAd("org.Mengine.Plugin.AdMob.RewardedAd.MengineAdMobRewardedAd", adService, this, adUnitId);
            }
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_ADMOB_REWARDEDINTERSTITIALAD == true) {
            String adUnitId = adUnitIds.get("rewarded_interstitial");

            if (adUnitId != null) {
                m_rewardedInterstitialAd = this.createAd("org.Mengine.Plugin.AdMob.RewardedInterstitialAd.MengineAdMobRewardedInterstitialAd", adService, this, adUnitId);
            }
        }
    }

    protected void initializeAdMob(@NonNull MengineApplication application, @NonNull MengineAdService adService) {
        if (m_initializationAttempted == true) {
            return;
        }

        m_initializationAttempted = true;

        JSONObject config = MengineFragmentRemoteConfig.INSTANCE.getRemoteConfig("admob_ad_units");

        if (config == null) {
            this.logInfo("[AdMob] no cached Remote Config admob_ad_units, skipping initialization for this session");

            adService.readyAdProvider();

            return;
        }

        Map<String, String> adUnitIds = new HashMap<>();
        String[] formats = {"banner", "topper", "interstitial", "rewarded", "rewarded_interstitial"};

        for (String format : formats) {
            Object value = config.opt(format);

            if (value == null) {
                continue;
            }

            if ((value instanceof String) == false) {
                this.logError("[AdMob] Remote Config admob_ad_units.%s must be a string", format);

                adService.readyAdProvider();

                return;
            }

            String adUnitId = (String)value;

            if (adUnitId.isEmpty() == true) {
                continue;
            }

            adUnitIds.put(format, adUnitId);
        }

        m_adUnitIds = Map.copyOf(adUnitIds);

        VersionInfo admobSdkVersion = MobileAds.getVersion();

        String admobSdkVersionString = String.valueOf(admobSdkVersion);

        this.logInfo("[AdMob SDK] version: %s", admobSdkVersionString);

        MobileAds.initialize(application, initializationStatus -> {
            MengineUtils.performOnMainThread(() -> {
                if (m_applicationReady == false) {
                    return;
                }

                m_adMobSdkInitialized = true;

                MengineActivity activity = this.getMengineActivity();

                if (activity != null) {
                    this.initializeAds(activity);
                }

                adService.readyAdProvider();
            });
        });
    }

    @Override
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, @NonNull MengineParamTransparencyConsent tcParam) {
        if (tcParam.isPending() == true) {
            return;
        }

        if (m_applicationReady == false) {
            return;
        }

        MengineAdService adService = application.getService(MengineAdService.class);

        if (adService == null) {
            return;
        }

        MengineUtils.performOnMainThread(() -> {
            this.initializeAdMob(application, adService);
        });
    }
}
