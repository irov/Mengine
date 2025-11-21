package org.Mengine.Plugin.AppLovin.MediationAmazon;

import android.content.res.Resources;
import android.util.DisplayMetrics;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

import com.amazon.device.ads.AdError;
import com.amazon.device.ads.DTBAdCallback;
import com.amazon.device.ads.DTBAdNetwork;
import com.amazon.device.ads.DTBAdNetworkInfo;
import com.amazon.device.ads.DTBAdRequest;
import com.amazon.device.ads.DTBAdResponse;
import com.amazon.device.ads.DTBAdSize;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxAppOpenAd;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;
import com.applovin.mediation.nativeAds.MaxNativeAdLoader;
import com.applovin.sdk.AppLovinSdkUtils;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinMediationInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinMediationLoadedCallback;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinPluginInterface;
import org.json.JSONObject;

import java.util.Map;

public class MengineAppLovinMediationAmazon implements MengineAppLovinMediationInterface {
    public static final @StringRes int MEDIATION_METADATA_BANNER_SLOTUUID = R.string.mengine_applovin_amazon_banner_slotuuid;
    public static final @StringRes int MEDIATION_METADATA_INTERSTITIAL_SLOTUUID = R.string.mengine_applovin_amazon_interstitial_slotuuid;
    public static final @StringRes int MEDIATION_METADATA_REWARDED_SLOTUUID = R.string.mengine_applovin_amazon_rewarded_slotuuid;

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
    public void onAppCreate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin) {
        //Empty
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin) {
        m_loadBannerStatus = ELoadAdStatus.ADLOAD_CANCEL;
        m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_CANCEL;
        m_loadRewardedStatus = ELoadAdStatus.ADLOAD_CANCEL;
    }

    @Override
    public void onMengineRemoteConfigFetch(@NonNull MengineApplication application, boolean updated, @NonNull Map<String, JSONObject> configs, @NonNull Map<String, Integer> ids) {
        // ToDo
    }

    @Override
    public void loadMediatorBanner(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxAdView adView, MengineAppLovinMediationLoadedCallback loadedCallback) throws MengineServiceInvalidInitializeException {
        String MengineAppLovinPlugin_AmazonBannerSlotId = plugin.getResourceString(MEDIATION_METADATA_BANNER_SLOTUUID);

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

                loadedCallback.onAdLoadFailed(MengineAppLovinMediationAmazon.this, new RuntimeException("Amazon failure load banner slotId: " + MengineAppLovinPlugin_AmazonBannerSlotId + " code: " + adErrorCode.ordinal() + " message: " + adErrorMessage));
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

                loadedCallback.onAdLoaded(MengineAppLovinMediationAmazon.this);
            }
        });
    }

    public void loadMediatorInterstitial(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxInterstitialAd interstitialAd, MengineAppLovinMediationLoadedCallback loadedCallback) throws MengineServiceInvalidInitializeException {
        switch (m_loadInterstitialStatus) {
            case ADLOAD_NONE:
                String MengineAppLovinPlugin_AmazonInterstitialSlotId = plugin.getResourceString(MEDIATION_METADATA_INTERSTITIAL_SLOTUUID);

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

                        loadedCallback.onAdLoadFailed(MengineAppLovinMediationAmazon.this, new RuntimeException("Amazon failure load interstitial slotId: " + MengineAppLovinPlugin_AmazonInterstitialSlotId + " code: " + adErrorCode.ordinal() + " message: " + adErrorMessage));
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

                        loadedCallback.onAdLoaded(MengineAppLovinMediationAmazon.this);
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

    public void loadMediatorRewarded(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxRewardedAd rewardedAd, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException {
        switch (m_loadRewardedStatus) {
            case ADLOAD_NONE:
                String MengineAppLovinPlugin_AmazonRewardedSlotId = plugin.getResourceString(MEDIATION_METADATA_REWARDED_SLOTUUID);

                m_loadRewardedStatus = ELoadAdStatus.ADLOAD_PROCESS;

                plugin.logInfo("Amazon try load rewarded slotId: %s"
                    , MengineUtils.getRedactedValue(MengineAppLovinPlugin_AmazonRewardedSlotId)
                );

                DTBAdNetworkInfo adNetworkInfo = new DTBAdNetworkInfo(DTBAdNetwork.MAX);
                DTBAdRequest loader = new DTBAdRequest(adNetworkInfo);

                // Switch video player width and height values(320, 480) depending on device orientation
                Resources resources = activity.getResources();
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

                        loadAdCallback.onAdLoadFailed(MengineAppLovinMediationAmazon.this, new RuntimeException("Amazon failure load rewarded slotId: " + MengineAppLovinPlugin_AmazonRewardedSlotId + " code: " + adErrorCode.ordinal() + " message: " + adErrorMessage));
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

                        loadAdCallback.onAdLoaded(MengineAppLovinMediationAmazon.this);
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

    @Override
    public void loadMediatorMREC(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxAdView adView, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException {
        loadAdCallback.onAdLoaded(this);
    }

    @Override
    public void loadMediatorNative(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxNativeAdLoader adLoader, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException {
        loadAdCallback.onAdLoaded(this);
    }

    @Override
    public void loadMediatorAppOpen(@NonNull MengineActivity activity, @NonNull MengineAppLovinPluginInterface plugin, @NonNull MaxAppOpenAd appOpenAd, MengineAppLovinMediationLoadedCallback loadAdCallback) throws MengineServiceInvalidInitializeException {
        loadAdCallback.onAdLoaded(this);
    }

}
