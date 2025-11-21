package org.Mengine.Plugin.AdMob;

import android.os.Bundle;

import androidx.annotation.BoolRes;
import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.VersionInfo;
import com.google.android.gms.ads.initialization.InitializationStatus;
import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdProviderInterface;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineApplication;
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

import java.util.ArrayList;
import java.util.List;

public class MengineAdMobPlugin extends MengineService implements MengineAdMobPluginInterface, MengineAdProviderInterface, MengineListenerApplication, MengineListenerActivity, MengineListenerTransparencyConsent {
    public static final String SERVICE_NAME = "AdMob";
    public static final boolean SERVICE_EMBEDDING = true;

    private volatile boolean m_adMobSdkInitialized = false;

    private MengineAdMobBannerAdInterface m_bannerAd;
    private MengineAdMobInterstitialAdInterface m_interstitialAd;
    private MengineAdMobRewardedAdInterface m_rewardedAd;

    private final List<MengineAdMobAdInterface> m_ads = new ArrayList<>();

    @SuppressWarnings("unchecked")
    protected <T extends MengineAdMobAdInterface> T createAd(@NonNull MengineAdService adService, @NonNull String className) throws MengineServiceInvalidInitializeException {
        T ad = (T)this.newInstance(className, true, adService, this);

        if (ad == null) {
            this.invalidInitialize("not found AdMob extension ad: %s"
                , className
            );
        }

        return ad;
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        MengineAdService adService = application.getService(MengineAdService.class);

        boolean noAds = adService.getNoAds();

        if (BuildConfig.MENGINE_APP_PLUGIN_ADMOB_BANNERAD == true && noAds == false) {
            m_bannerAd = this.createAd(adService, "org.Mengine.Plugin.AdMob.BannerAd.MengineAdMobBannerAd");

            m_ads.add(m_bannerAd);
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_ADMOB_INTERSTITIALAD == true && noAds == false) {
            m_interstitialAd = this.createAd(adService, "org.Mengine.Plugin.AdMob.InterstitialAd.MengineAdMobInterstitialAd");

            m_ads.add(m_interstitialAd);
        }

        if (BuildConfig.MENGINE_APP_PLUGIN_ADMOB_REWARDEDAD == true) {
            m_rewardedAd = this.createAd(adService, "org.Mengine.Plugin.AdMob.RewardedAd.MengineAdMobRewardedAd");

            m_ads.add(m_rewardedAd);
        }

        adService.setAdProvider(this);
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        m_bannerAd = null;
        m_interstitialAd = null;
        m_rewardedAd = null;

        m_ads.clear();
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        if (m_adMobSdkInitialized == true) {
            for (MengineAdMobAdInterface ad : m_ads) {
                ad.onActivityCreate(activity);
            }
        }
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        if (m_adMobSdkInitialized == true) {
            for (MengineAdMobAdInterface ad : m_ads) {
                ad.onActivityDestroy(activity);
            }
        }
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
            this.logWarning("not found banner");

            return;
        }

        this.logInfo("banner show");

        m_bannerAd.show();
    }

    @Override
    public void hideBanner() {
        if (m_bannerAd == null) {
            this.logWarning("not found banner");

            return;
        }

        this.logInfo("banner hide");

        m_bannerAd.hide();
    }

    @Override
    public int getBannerWidth() {
        if (m_bannerAd == null) {
            return 0;
        }

        int widthPx = m_bannerAd.getWidthPx();

        return widthPx;
    }

    @Override
    public int getBannerHeight() {
        if (m_bannerAd == null) {
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

    protected void initializeAdMob(@NonNull MengineApplication application, @NonNull MengineAdService adService) {
        if (m_adMobSdkInitialized == true) {
            return;
        }

        VersionInfo admobSdkVersion = MobileAds.getVersion();

        String admobSdkVersionString = String.valueOf(admobSdkVersion);

        this.logInfo("[AdMob SDK] version: %s", admobSdkVersionString);

        MobileAds.initialize(application, new OnInitializationCompleteListener() {
            @Override
            public void onInitializationComplete(@NonNull InitializationStatus initializationStatus) {
                m_adMobSdkInitialized = true;

                MengineActivity activity = MengineAdMobPlugin.this.getMengineActivity();

                if (activity != null) {
                    for (MengineAdMobAdInterface ad : m_ads) {
                        ad.onActivityCreate(activity);
                    }
                }

                adService.readyAdProvider();
            }
        });
    }

    @Override
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, @NonNull MengineParamTransparencyConsent tcParam) {
        if (m_adMobSdkInitialized == true) {
            return;
        }

        MengineAdService adService = application.getService(MengineAdService.class);

        if (adService == null) {
            return;
        }

        this.initializeAdMob(application, adService);
    }
}
