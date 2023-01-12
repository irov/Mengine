package org.Mengine.Plugin.AppLovin;

import org.Mengine.Plugin.AppLovin.MengineAppLovinMediationInterface;

import org.Mengine.Base.MengineActivity;

import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;

import com.amazon.device.ads.AdError;
import com.amazon.device.ads.AdRegistration;
import com.amazon.device.ads.DTBAdCallback;
import com.amazon.device.ads.DTBAdNetwork;
import com.amazon.device.ads.DTBAdNetworkInfo;
import com.amazon.device.ads.DTBAdRequest;
import com.amazon.device.ads.DTBAdResponse;
import com.amazon.device.ads.DTBAdSize;
import com.amazon.device.ads.MRAIDPolicy;

import android.content.Context;
import androidx.annotation.NonNull;

public class MengineAppLovinMediationAmazon implements MengineAppLovinMediationInterface {
    enum ELoadAdStatus {
        ADLOAD_NONE,
        ADLOAD_AMAZON,
        ADLOAD_READY
    }

    private ELoadAdStatus m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_NONE;
    private ELoadAdStatus m_loadRewardedStatus = ELoadAdStatus.ADLOAD_NONE;

    private String m_bannerSlotId;
    private String m_interstitialSlotId;
    private String m_rewardedSlotId;

    public void initializeMediator(MengineActivity activity, String appId, String bannerSlotId, String interstitialSlotId, String rewardedSlotId) throws Exception {
        final Context context = activity.getBaseContext();

        AdRegistration.getInstance(appId, activity);
        AdRegistration.setAdNetworkInfo(new DTBAdNetworkInfo(DTBAdNetwork.MAX));
        AdRegistration.setMRAIDSupportedVersions(new String[]{"1.0", "2.0", "3.0"});
        AdRegistration.setMRAIDPolicy(MRAIDPolicy.CUSTOM);

        boolean MengineAppLovinPlugin_AmazonEnableLogging = activity.getMetaDataBoolean("MengineAppLovinPlugin_AmazonEnableTesting", false);

        if (MengineAppLovinPlugin_AmazonEnableLogging == true) {
            AdRegistration.enableTesting(true);
        }

        boolean MengineAppLovinPlugin_AmazonEnableLogging = activity.getMetaDataBoolean("MengineAppLovinPlugin_AmazonEnableLogging", false);

        if (MengineAppLovinPlugin_AmazonEnableLogging == true || BuildConfig.DEBUG == true ) {
            AdRegistration.enableLogging(true);
        }

        m_bannerSlotId = bannerSlotId;
        m_interstitialSlotId = interstitialSlotId;
        m_rewardedSlotId = rewardedSlotId;
    }

    public void initializeMediatorBanner(MengineActivity activity, MaxAdView adView) throws Exception {
        if (m_bannerSlotId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] AmazonBannerSlotId");
        }

        int width = activity.getWindow().getDecorView().getWidth();
        int height = activity.getWindow().getDecorView().getHeight();

        DTBAdSize size = new DTBAdSize(width, height, m_bannerSlotId);

        DTBAdRequest adLoader = new DTBAdRequest();
        adLoader.setSizes(size);
        adLoader.loadAd(new DTBAdCallback() {
            @Override
            public void onFailure(@NonNull AdError adError) {
                // 'adView' is your instance of MaxAdView
                adView.setLocalExtraParameter("amazon_ad_error", adError);
                adView.loadAd();
            }

            @Override
            public void onSuccess(@NonNull DTBAdResponse dtbAdResponse) {
                // 'adView' is your instance of MaxAdView
                adView.setLocalExtraParameter("amazon_ad_response", dtbAdResponse);
                adView.loadAd();
            }
        });
    }

    public void loadMediatorInterstitial(MengineActivity activity, MaxInterstitialAd interstitialAd) throws Exception {
        switch (m_loadInterstitialStatus) {
            case ADLOAD_NONE:
                if (m_interstitialSlotId.isEmpty() == true) {
                    throw new Exception("Need to add config value for [AppLovin] AmazonInterstitialSlotId");
                }

                m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_AMAZON;

                DTBAdRequest adLoader = new DTBAdRequest();
                adLoader.setSizes(new DTBAdSize.DTBInterstitialAdSize(m_interstitialSlotId));
                adLoader.loadAd(new DTBAdCallback() {
                    @Override
                    public void onSuccess(@NonNull final DTBAdResponse dtbAdResponse) {
                        // 'interstitialAd' is your instance of MaxInterstitialAd
                        interstitialAd.setLocalExtraParameter("amazon_ad_response", dtbAdResponse);
                        interstitialAd.loadAd();

                        m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_READY;
                    }

                    @Override
                    public void onFailure(@NonNull final AdError adError) {
                        // 'interstitialAd' is your instance of MaxInterstitialAd
                        interstitialAd.setLocalExtraParameter("amazon_ad_error", adError);
                        interstitialAd.loadAd();

                        m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_READY;
                    }
                });
                break;
            case ADLOAD_AMAZON:
                break;
            case ADLOAD_READY:
                interstitialAd.loadAd();
                break;
        }
    }

    public void loadMediatorRewarded(MengineActivity activity, MaxRewardedAd rewardedAd) throws Exception {
        switch (m_loadRewardedStatus) {
            case ADLOAD_NONE:
                if (m_rewardedSlotId.isEmpty() == true) {
                    throw new Exception("Need to add config value for [AppLovin] AmazonVideoRewardedSlotId");
                }

                m_loadRewardedStatus = ELoadAdStatus.ADLOAD_AMAZON;

                DTBAdRequest loader = new DTBAdRequest();
                // Switch video player width and height values(320, 480) depending on device orientation

                int width = activity.getWindow().getDecorView().getWidth();
                int height = activity.getWindow().getDecorView().getHeight();

                loader.setSizes(new DTBAdSize.DTBVideo(width, height, m_rewardedSlotId));
                loader.loadAd(new DTBAdCallback() {
                    @Override
                    public void onSuccess(@NonNull final DTBAdResponse dtbAdResponse) {
                        // 'rewardedAd' is your instance of MaxRewardedAd
                        rewardedAd.setLocalExtraParameter("amazon_ad_response", dtbAdResponse);
                        rewardedAd.loadAd();

                        m_loadRewardedStatus = ELoadAdStatus.ADLOAD_READY;
                    }

                    @Override
                    public void onFailure(@NonNull final AdError adError) {
                        // 'rewardedAd' is your instance of MaxRewardedAd
                        rewardedAd.setLocalExtraParameter("amazon_ad_error", adError);
                        rewardedAd.loadAd();

                        m_loadRewardedStatus = ELoadAdStatus.ADLOAD_READY;
                    }
                });
                break;
            case ADLOAD_AMAZON:
                break;
            case ADLOAD_READY:
                rewardedAd.loadAd();
                break;
        }
    }
}
