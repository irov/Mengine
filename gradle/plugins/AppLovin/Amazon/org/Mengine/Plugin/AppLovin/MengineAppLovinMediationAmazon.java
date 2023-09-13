package org.Mengine.Plugin.AppLovin;

import androidx.annotation.NonNull;

import com.amazon.device.ads.AdError;
import com.amazon.device.ads.DTBAdCallback;
import com.amazon.device.ads.DTBAdRequest;
import com.amazon.device.ads.DTBAdResponse;
import com.amazon.device.ads.DTBAdSize;
import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineLog;

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
    public void initializeMediator(MengineActivity activity, MengineAppLovinPlugin plugin) {
        //Empty
    }

    @Override
    public void finalizeMediator(MengineActivity activity, MengineAppLovinPlugin plugin) {
        m_loadBannerStatus = ELoadAdStatus.ADLOAD_CANCEL;
        m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_CANCEL;
        m_loadRewardedStatus = ELoadAdStatus.ADLOAD_CANCEL;
    }

    @Override
    public void initializeMediatorBanner(MengineActivity activity, MengineAppLovinPlugin plugin, MaxAdView adView, MengineAppLovinMediationLoadAdCallback loadAdCallback) {
        String MengineAppLovinPlugin_AmazonBannerSlotId = activity.getMetaDataString(MEDIATION_METADATA_BANNER_SLOT_ID);

        if (MengineAppLovinPlugin_AmazonBannerSlotId == null) {
            String message = MengineLog.buildTotalMsg("MengineAppLovinMediationAmazon need to add config value for [%s]", MEDIATION_METADATA_BANNER_SLOT_ID);

            throw new RuntimeException(message);
        }

        int width = activity.getWindow().getDecorView().getWidth();
        int height = activity.getWindow().getDecorView().getHeight();

        m_loadBannerStatus = ELoadAdStatus.ADLOAD_PROCESS;

        plugin.logMessage("Amazon try load banner [%d:%d] [%s]"
            , width
            , height
            , MengineAppLovinPlugin_AmazonBannerSlotId
        );

        DTBAdSize size = new DTBAdSize(width, height, MengineAppLovinPlugin_AmazonBannerSlotId);

        DTBAdRequest adLoader = new DTBAdRequest();
        adLoader.setSizes(size);
        adLoader.loadAd(new DTBAdCallback() {
            @Override
            public void onFailure(@NonNull AdError adError) {
                if (m_loadBannerStatus == ELoadAdStatus.ADLOAD_CANCEL) {
                    return;
                }

                m_loadBannerStatus = ELoadAdStatus.ADLOAD_FAILURE;

                plugin.logError("[ERROR] Amazon failure load banner [%s] code [%s] message: %s"
                    , MengineAppLovinPlugin_AmazonBannerSlotId
                    , adError.getCode()
                    , adError.getMessage()
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

                plugin.logMessage("Amazon successful load banner [%s]"
                    , MengineAppLovinPlugin_AmazonBannerSlotId
                );

                adView.setLocalExtraParameter("amazon_ad_response", dtbAdResponse);

                loadAdCallback.onLoadAd();
            }
        });
    }

    public void loadMediatorInterstitial(MengineActivity activity, MengineAppLovinPlugin plugin, MaxInterstitialAd interstitialAd, MengineAppLovinMediationLoadAdCallback loadAdCallback)  {
        switch (m_loadInterstitialStatus) {
            case ADLOAD_NONE:
                String MengineAppLovinPlugin_AmazonInterstitialSlotId = activity.getMetaDataString(MEDIATION_METADATA_INTERSTITIAL_SLOT_ID);

                if (MengineAppLovinPlugin_AmazonInterstitialSlotId == null) {
                    String message = MengineLog.buildTotalMsg("MengineAppLovinMediationAmazon need to add config value for [%s]", MEDIATION_METADATA_INTERSTITIAL_SLOT_ID);

                    throw new RuntimeException(message);
                }

                m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_PROCESS;

                plugin.logMessage("Amazon try load interstitial [%s]"
                    , MengineAppLovinPlugin_AmazonInterstitialSlotId
                );

                DTBAdRequest adLoader = new DTBAdRequest();
                adLoader.setSizes(new DTBAdSize.DTBInterstitialAdSize(MengineAppLovinPlugin_AmazonInterstitialSlotId));
                adLoader.loadAd(new DTBAdCallback() {
                    @Override
                    public void onFailure(@NonNull final AdError adError) {
                        if (m_loadInterstitialStatus == ELoadAdStatus.ADLOAD_CANCEL) {
                            return;
                        }

                        m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_FAILURE;

                        plugin.logError("[ERROR] Amazon failure load interstitial [%s] code [%s] message: %s"
                            , MengineAppLovinPlugin_AmazonInterstitialSlotId
                            , adError.getCode()
                            , adError.getMessage()
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

                        plugin.logMessage("Amazon successful load interstitial [%s]"
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

    public void loadMediatorRewarded(MengineActivity activity, MengineAppLovinPlugin plugin, MaxRewardedAd rewardedAd, MengineAppLovinMediationLoadAdCallback loadAdCallback) {
        switch (m_loadRewardedStatus) {
            case ADLOAD_NONE:
                String MengineAppLovinPlugin_AmazonRewardedSlotId = activity.getMetaDataString(MEDIATION_METADATA_REWARDED_SLOT_ID);

                if (MengineAppLovinPlugin_AmazonRewardedSlotId == null) {
                    String message = MengineLog.buildTotalMsg("MengineAppLovinMediationAmazon need to add config value for [%s]", MEDIATION_METADATA_REWARDED_SLOT_ID);

                    throw new RuntimeException(message);
                }

                m_loadRewardedStatus = ELoadAdStatus.ADLOAD_PROCESS;

                plugin.logMessage("Amazon try load rewarded [%s]"
                    , MengineAppLovinPlugin_AmazonRewardedSlotId
                );

                DTBAdRequest loader = new DTBAdRequest();

                // Switch video player width and height values(320, 480) depending on device orientation
                int width = activity.getWindow().getDecorView().getWidth();
                int height = activity.getWindow().getDecorView().getHeight();

                loader.setSizes(new DTBAdSize.DTBVideo(width, height, MengineAppLovinPlugin_AmazonRewardedSlotId));
                loader.loadAd(new DTBAdCallback() {
                    @Override
                    public void onFailure(@NonNull final AdError adError) {
                        if (m_loadRewardedStatus == ELoadAdStatus.ADLOAD_CANCEL) {
                            return;
                        }

                        m_loadRewardedStatus = ELoadAdStatus.ADLOAD_FAILURE;

                        plugin.logError("[ERROR] Amazon failure load rewarded [%s] code [%s] message: %s"
                            , MengineAppLovinPlugin_AmazonRewardedSlotId
                            , adError.getCode()
                            , adError.getMessage()
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

                        plugin.logMessage("Amazon successful load rewarded [%s]"
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
