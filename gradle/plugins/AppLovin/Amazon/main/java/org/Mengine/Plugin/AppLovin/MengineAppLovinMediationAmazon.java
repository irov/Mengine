package org.Mengine.Plugin.AppLovin;

import android.content.res.Resources;
import android.util.DisplayMetrics;

import androidx.annotation.NonNull;

import com.amazon.device.ads.AdError;
import com.amazon.device.ads.DTBAdCallback;
import com.amazon.device.ads.DTBAdNetwork;
import com.amazon.device.ads.DTBAdNetworkInfo;
import com.amazon.device.ads.DTBAdRequest;
import com.amazon.device.ads.DTBAdResponse;
import com.amazon.device.ads.DTBAdSize;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;
import com.applovin.sdk.AppLovinSdkUtils;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

public class MengineAppLovinMediationAmazon implements MengineAppLovinMediationInterface {
    public static final String MEDIATION_METADATA_BANNER_SLOT_ID = "mengine.applovin.amazon.banner_slot_id";
    public static final String MEDIATION_METADATA_INTERSTITIAL_SLOT_ID = "mengine.applovin.amazon.interstitial_slot_id";
    public static final String MEDIATION_METADATA_REWARDED_SLOT_ID = "mengine.applovin.amazon.rewarded_slot_id";

    enum ELoadAdStatus {
        ADLOAD_NONE,
        ADLOAD_PROCESS,
        ADLOAD_SUCCESSFUL,
        ADLOAD_FAILURE,
        ADLOAD_CANCEL
    }

    private ELoadAdStatus m_loadBannerStatus = ELoadAdStatus.ADLOAD_NONE;
    private ELoadAdStatus m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_NONE;
    private ELoadAdStatus m_loadRewardedStatus = ELoadAdStatus.ADLOAD_NONE;

    @Override
    public void initializeMediator(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin) {
        //Empty
    }

    @Override
    public void finalizeMediator(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin) {
        m_loadBannerStatus = ELoadAdStatus.ADLOAD_CANCEL;
        m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_CANCEL;
        m_loadRewardedStatus = ELoadAdStatus.ADLOAD_CANCEL;
    }

    @Override
    public void initializeMediatorBanner(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin, @NonNull MaxAdView adView, MengineAppLovinMediationLoadAdCallback loadAdCallback) throws MengineServiceInvalidInitializeException {
        String MengineAppLovinPlugin_AmazonBannerSlotId = plugin.getMetaDataString(MEDIATION_METADATA_BANNER_SLOT_ID);

        m_loadBannerStatus = ELoadAdStatus.ADLOAD_PROCESS;

        MaxAdFormat adFormat = MaxAdFormat.BANNER;

        AppLovinSdkUtils.Size adSize = adFormat.getSize();

        int width = adSize.getWidth();
        int height = adSize.getHeight();

        plugin.logInfo("Amazon try load banner: %d:%d slotId: %s"
            , width
            , height
            , MengineUtils.getRedactedValue(MengineAppLovinPlugin_AmazonBannerSlotId)
        );

        DTBAdSize size = new DTBAdSize(width, height, MengineAppLovinPlugin_AmazonBannerSlotId);

        DTBAdNetworkInfo adNetworkInfo = new DTBAdNetworkInfo(DTBAdNetwork.MAX);
        DTBAdRequest adLoader = new DTBAdRequest(adNetworkInfo);

        adLoader.setSizes(size);

        adLoader.loadAd(new DTBAdCallback() {
            @Override
            public void onFailure(@NonNull AdError adError) {
                if (m_loadBannerStatus == ELoadAdStatus.ADLOAD_CANCEL) {
                    return;
                }

                m_loadBannerStatus = ELoadAdStatus.ADLOAD_FAILURE;

                AdError.ErrorCode adErrorCode = adError.getCode();
                String adErrorMessage = adError.getMessage();

                plugin.logError("[ERROR] Amazon failure load banner slotId: %s code: %s message: %s"
                    , MengineAppLovinPlugin_AmazonBannerSlotId
                    , adErrorCode.ordinal()
                    , adErrorMessage
                );

                adView.setLocalExtraParameter("amazon_ad_error", adError);

                loadAdCallback.onLoadAd();
            }

            @Override
            public void onSuccess(@NonNull DTBAdResponse dtbAdResponse) {
                if (m_loadBannerStatus == ELoadAdStatus.ADLOAD_CANCEL) {
                    return;
                }

                m_loadBannerStatus = ELoadAdStatus.ADLOAD_SUCCESSFUL;

                plugin.logMessage("Amazon success load banner slotId: %s"
                    , MengineAppLovinPlugin_AmazonBannerSlotId
                );

                adView.setLocalExtraParameter("amazon_ad_response", dtbAdResponse);

                loadAdCallback.onLoadAd();
            }
        });
    }

    public void loadMediatorInterstitial(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin, @NonNull MaxInterstitialAd interstitialAd, MengineAppLovinMediationLoadAdCallback loadAdCallback) throws MengineServiceInvalidInitializeException {
        switch (m_loadInterstitialStatus) {
            case ADLOAD_NONE:
                String MengineAppLovinPlugin_AmazonInterstitialSlotId = plugin.getMetaDataString(MEDIATION_METADATA_INTERSTITIAL_SLOT_ID);

                m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_PROCESS;

                plugin.logInfo("Amazon try load interstitial slotId: %s"
                    , MengineUtils.getRedactedValue(MengineAppLovinPlugin_AmazonInterstitialSlotId)
                );

                DTBAdNetworkInfo adNetworkInfo = new DTBAdNetworkInfo(DTBAdNetwork.MAX);
                DTBAdRequest adLoader = new DTBAdRequest(adNetworkInfo);

                DTBAdSize.DTBInterstitialAdSize adSizes = new DTBAdSize.DTBInterstitialAdSize(MengineAppLovinPlugin_AmazonInterstitialSlotId);

                adLoader.setSizes(adSizes);

                adLoader.loadAd(new DTBAdCallback() {
                    @Override
                    public void onFailure(@NonNull final AdError adError) {
                        if (m_loadInterstitialStatus == ELoadAdStatus.ADLOAD_CANCEL) {
                            return;
                        }

                        m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_FAILURE;

                        AdError.ErrorCode adErrorCode = adError.getCode();
                        String adErrorMessage = adError.getMessage();

                        plugin.logError("[ERROR] Amazon failure load interstitial slotId: %s code: %s message: %s"
                            , MengineAppLovinPlugin_AmazonInterstitialSlotId
                            , adErrorCode.ordinal()
                            , adErrorMessage
                        );

                        interstitialAd.setLocalExtraParameter("amazon_ad_error", adError);

                        loadAdCallback.onLoadAd();
                    }

                    @Override
                    public void onSuccess(@NonNull final DTBAdResponse dtbAdResponse) {
                        if (m_loadInterstitialStatus == ELoadAdStatus.ADLOAD_CANCEL) {
                            return;
                        }

                        m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_SUCCESSFUL;

                        plugin.logMessage("Amazon success load interstitial slotId: %s"
                            , MengineAppLovinPlugin_AmazonInterstitialSlotId
                        );

                        interstitialAd.setLocalExtraParameter("amazon_ad_response", dtbAdResponse);

                        loadAdCallback.onLoadAd();
                    }
                });
                break;
            case ADLOAD_PROCESS:
                break;
            case ADLOAD_SUCCESSFUL:
            case ADLOAD_FAILURE:
                interstitialAd.loadAd();
                break;
        }
    }

    public void loadMediatorRewarded(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin, @NonNull MaxRewardedAd rewardedAd, MengineAppLovinMediationLoadAdCallback loadAdCallback) throws MengineServiceInvalidInitializeException {
        switch (m_loadRewardedStatus) {
            case ADLOAD_NONE:
                String MengineAppLovinPlugin_AmazonRewardedSlotId = plugin.getMetaDataString(MEDIATION_METADATA_REWARDED_SLOT_ID);

                m_loadRewardedStatus = ELoadAdStatus.ADLOAD_PROCESS;

                plugin.logInfo("Amazon try load rewarded slotId: %s"
                    , MengineUtils.getRedactedValue(MengineAppLovinPlugin_AmazonRewardedSlotId)
                );

                DTBAdNetworkInfo adNetworkInfo = new DTBAdNetworkInfo(DTBAdNetwork.MAX);
                DTBAdRequest loader = new DTBAdRequest(adNetworkInfo);

                // Switch video player width and height values(320, 480) depending on device orientation
                Resources resources = application.getResources();
                DisplayMetrics metrics = resources.getDisplayMetrics();

                int width = metrics.widthPixels;
                int height = metrics.heightPixels;

                loader.setSizes(new DTBAdSize.DTBVideo(width, height, MengineAppLovinPlugin_AmazonRewardedSlotId));

                loader.loadAd(new DTBAdCallback() {
                    @Override
                    public void onFailure(@NonNull final AdError adError) {
                        if (m_loadRewardedStatus == ELoadAdStatus.ADLOAD_CANCEL) {
                            return;
                        }

                        m_loadRewardedStatus = ELoadAdStatus.ADLOAD_FAILURE;

                        AdError.ErrorCode adErrorCode = adError.getCode();
                        String adErrorMessage = adError.getMessage();

                        plugin.logError("[ERROR] Amazon failure load rewarded slotId: %s code: %s message: %s"
                            , MengineAppLovinPlugin_AmazonRewardedSlotId
                            , adErrorCode.ordinal()
                            , adErrorMessage
                        );

                        rewardedAd.setLocalExtraParameter("amazon_ad_error", adError);

                        loadAdCallback.onLoadAd();
                    }

                    @Override
                    public void onSuccess(@NonNull final DTBAdResponse dtbAdResponse) {
                        if (m_loadRewardedStatus == ELoadAdStatus.ADLOAD_CANCEL) {
                            return;
                        }

                        m_loadRewardedStatus = ELoadAdStatus.ADLOAD_SUCCESSFUL;

                        plugin.logMessage("Amazon success load rewarded slotId: %s"
                            , MengineAppLovinPlugin_AmazonRewardedSlotId
                        );

                        rewardedAd.setLocalExtraParameter("amazon_ad_response", dtbAdResponse);

                        loadAdCallback.onLoadAd();
                    }
                });
                break;
            case ADLOAD_PROCESS:
                break;
            case ADLOAD_SUCCESSFUL:
            case ADLOAD_FAILURE:
                rewardedAd.loadAd();
                break;
        }
    }
}
