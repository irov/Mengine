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

    public void initializeMediator(MengineActivity activity) throws Exception {
        final Context context = activity.getBaseContext();

        String AppLovin_AmazonAppId = activity.getConfigValue("AppLovin", "AmazonAppId", "");

        if (AppLovin_AmazonAppId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] AmazonAppId");
        }

        AdRegistration.getInstance(AppLovin_AmazonAppId, activity);
        AdRegistration.setAdNetworkInfo(new DTBAdNetworkInfo(DTBAdNetwork.MAX));
        AdRegistration.setMRAIDSupportedVersions(new String[]{"1.0", "2.0", "3.0"});
        AdRegistration.setMRAIDPolicy(MRAIDPolicy.CUSTOM);

        boolean AppLovin_AmazonEnableTesting = activity.getConfigValueBoolean("AppLovin", "AmazonEnableTesting", false);

        if (AppLovin_AmazonEnableTesting == true) {
            AdRegistration.enableTesting(true);
        }

        boolean AppLovin_AmazonEnableLogging = activity.getConfigValueBoolean("AppLovin", "AmazonEnableLogging", false);

        if (AppLovin_AmazonEnableLogging == true || BuildConfig.DEBUG == true ) {
            AdRegistration.enableLogging(true);
        }
    }

    public void initializeMediatorBanner(MengineActivity activity, MaxAdView adView) throws Exception {
        String AppLovin_AmazonBannerSlotId = activity.getConfigValue("AppLovin", "AmazonBannerSlotId", "");

        if (AppLovin_AmazonBannerSlotId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] AmazonBannerSlotId");
        }

        int width = activity.getWindow().getDecorView().getWidth();
        int height = activity.getWindow().getDecorView().getHeight();

        DTBAdSize size = new DTBAdSize(width, height, AppLovin_AmazonBannerSlotId);

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
                final String AppLovin_AmazonInterstitialSlotId = activity.getConfigValue("AppLovin", "AmazonInterstitialSlotId", "");

                if (AppLovin_AmazonInterstitialSlotId.isEmpty() == true) {
                    throw new Exception("Need to add config value for [AppLovin] AmazonInterstitialSlotId");
                }

                m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_AMAZON;

                DTBAdRequest adLoader = new DTBAdRequest();
                adLoader.setSizes(new DTBAdSize.DTBInterstitialAdSize(AppLovin_AmazonInterstitialSlotId));
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
                final String AppLovin_AmazonRewardedSlotId = activity.getConfigValue("AppLovin", "AmazonRewardedSlotId", "");

                if (AppLovin_AmazonRewardedSlotId.isEmpty() == true) {
                    throw new Exception("Need to add config value for [AppLovin] AmazonVideoRewardedSlotId");
                }

                m_loadRewardedStatus = ELoadAdStatus.ADLOAD_AMAZON;

                DTBAdRequest loader = new DTBAdRequest();
                // Switch video player width and height values(320, 480) depending on device orientation

                int width = activity.getWindow().getDecorView().getWidth();
                int height = activity.getWindow().getDecorView().getHeight();

                loader.setSizes(new DTBAdSize.DTBVideo(width, height, AppLovin_AmazonRewardedSlotId));
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
