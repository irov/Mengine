package org.Mengine.Plugin.AppLovin;

import android.content.Context;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;

import com.amazon.device.ads.AdError;
import com.amazon.device.ads.AdRegistration;
import com.amazon.device.ads.DTBAdCallback;
import com.amazon.device.ads.DTBAdNetwork;
import com.amazon.device.ads.DTBAdNetworkInfo;
import com.amazon.device.ads.DTBAdRequest;
import com.amazon.device.ads.DTBAdResponse;
import com.amazon.device.ads.DTBAdSize;
import com.amazon.device.ads.MRAIDPolicy;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxAdViewAdListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxReward;
import com.applovin.mediation.MaxRewardedAdListener;
import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;
import com.applovin.sdk.AppLovinAdSize;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;
import com.applovin.sdk.AppLovinSdkUtils;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.ThreadUtil;

import java.util.concurrent.TimeUnit;

public class MengineAppLovinPlugin extends MenginePlugin {
    /**
     * <p>
     * void initialize()
     * - onApplovinPluginOnSdkInitialized
     * <p>
     * установка Interstitial
     * void initInterstitial()
     * void loadInterstitial()
     * void showInterstitial()
     * - onApplovinInterstitialOnAdLoaded
     * - onApplovinInterstitialOnAdDisplayed
     * - onApplovinInterstitialOnAdHidden
     * - onApplovinInterstitialOnAdClicked
     * - onApplovinInterstitialOnAdLoadFailed
     * - onApplovinInterstitialOnAdDisplayFailed
     * <p>
     * установка Rewarded
     * void initRewarded()
     * void loadRewarded()
     * void showRewarded()
     * - onApplovinRewardedOnRewardedVideoStarted
     * - onApplovinRewardedOnRewardedVideoCompleted
     * - onApplovinRewardedOnUserRewarded
     * - onApplovinRewardedOnAdLoaded
     * - onApplovinRewardedOnAdDisplayed
     * - onApplovinRewardedOnAdHidden
     * - onApplovinRewardedOnAdClicked
     * - onApplovinRewardedOnAdLoadFailed
     * - onApplovinRewardedOnAdDisplayFailed
     * <p>
     * установка Banner
     * void initBanner()
     * void bannerVisible(boolean show)
     * - onApplovinBannerOnAdDisplayed
     * - onApplovinBannerOnAdHidden
     * - onApplovinBannerOnAdClicked
     * - onApplovinBannerOnAdLoadFailed
     * - onApplovinBannerOnAdDisplayFailed
     * - onApplovinBannerOnAdExpanded
     * - onApplovinBannerOnAdCollapsed
     */

    enum ELoadAdStatus {
        ADLOAD_NONE,
        ADLOAD_AMAZON,
        ADLOAD_READY
    }

    private MaxInterstitialAd m_interstitialAd;
    private ELoadAdStatus m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_NONE;
    private int m_retryAttemptInterstitial;

    private MaxRewardedAd m_rewardedAd;
    private ELoadAdStatus m_loadRewardedStatus = ELoadAdStatus.ADLOAD_NONE;
    private int m_retryAttemptRewarded;

    private MaxAdView m_adView;

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("AppLovin");
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        final Context context = activity.getBaseContext();
        AppLovinSdk.getInstance(context).setMediationProvider("max");
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        if (m_interstitialAd != null) {
            m_interstitialAd.destroy();
            m_interstitialAd = null;
        }

        if (m_rewardedAd != null) {
            m_rewardedAd.destroy();
            m_rewardedAd = null;
        }

        if (m_adView != null) {
            m_adView.destroy();
            m_adView = null;
        }
    }

    public void initialize() {
        MengineActivity activity = this.getActivity();
        final Context context = activity.getBaseContext();

        String AppLovin_AmazonAppId = activity.getConfigValue("AppLovin", "AmazonAppId", "");

        if (AppLovin_AmazonAppId.isEmpty() == false) {
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

        AppLovinSdk.initializeSdk(context, new AppLovinSdk.SdkInitializationListener() {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration) {
                MengineAppLovinPlugin.this.logInfo("AppLovinSdk initialized: consent dialog [%s] country [%s]"
                        , configuration.getConsentDialogState().toString()
                        , configuration.getCountryCode()
                );

                MengineAppLovinPlugin.this.pythonCall("onApplovinPluginOnSdkInitialized");
            }
        });
    }

    public void initBanner() throws Exception {
        MengineActivity activity = this.getActivity();

        String AppLovin_BannerAdUnitId = activity.getConfigValue("AppLovin", "BannerAdUnitId", "");

        if (AppLovin_BannerAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] BannerAdUnitId");
        }

        m_adView = new MaxAdView(AppLovin_BannerAdUnitId, activity);

        MaxAdViewAdListener maxAdViewAdListener = new MaxAdViewAdListener() {
            @Override
            public void onAdLoaded(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdDisplayFailed");
            }

            @Override
            public void onAdExpanded(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdExpanded");
            }

            @Override
            public void onAdCollapsed(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdCollapsed");
            }
        };

        m_adView.setListener(maxAdViewAdListener);

        int width = ViewGroup.LayoutParams.MATCH_PARENT;
        int heightPx = AppLovinSdkUtils.dpToPx(activity, (AppLovinSdkUtils.isTablet(activity) ? AppLovinAdSize.LEADER : AppLovinAdSize.BANNER).getHeight());

        m_adView.setLayoutParams(new FrameLayout.LayoutParams(width, heightPx));

        int height = activity.getWindow().getDecorView().getHeight();

        m_adView.setTranslationY(height - heightPx);
        ViewGroup rootView = activity.getWindow().getDecorView().findViewById(android.R.id.content);
        rootView.addView(m_adView);

        String AppLovin_AmazonBannerSlotId = activity.getConfigValue("AppLovin", "AmazonBannerSlotId", "");

        if (AppLovin_AmazonBannerSlotId.isEmpty() == false) {
            DTBAdSize size = new DTBAdSize(activity.getWindow().getDecorView().getWidth(), height, AppLovin_AmazonBannerSlotId);

            DTBAdRequest adLoader = new DTBAdRequest();
            adLoader.setSizes(size);
            adLoader.loadAd(new DTBAdCallback() {
                @Override
                public void onFailure(@NonNull AdError adError) {
                    // 'adView' is your instance of MaxAdView
                    m_adView.setLocalExtraParameter("amazon_ad_error", adError);
                    m_adView.loadAd();
                }

                @Override
                public void onSuccess(@NonNull DTBAdResponse dtbAdResponse) {
                    // 'adView' is your instance of MaxAdView
                    m_adView.setLocalExtraParameter("amazon_ad_response", dtbAdResponse);
                    m_adView.loadAd();
                }
            });
        } else {
            // Load the ad
            m_adView.loadAd();
        }

        this.bannerVisible(false);
    }

    public void bannerVisible(boolean show) {
        if (show) {
            m_adView.startAutoRefresh();
            m_adView.setVisibility(View.VISIBLE);
        } else {
            m_adView.stopAutoRefresh();
            m_adView.setVisibility(View.GONE);
        }
    }

    public void initInterstitial() throws Exception {
        MengineActivity activity = this.getActivity();

        String AppLovin_InterstitialAdUnitId = activity.getConfigValue("AppLovin", "InterstitialAdUnitId", "");

        if (AppLovin_InterstitialAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] InterstitialAdUnitId");
        }

        m_interstitialAd = new MaxInterstitialAd(AppLovin_InterstitialAdUnitId, activity);

        MaxAdListener maxAdListener = new MaxAdListener() {
            @Override
            public void onAdLoaded(MaxAd ad) {
                m_retryAttemptInterstitial = 0;

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                m_interstitialAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                m_retryAttemptInterstitial++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptInterstitial)));

                ThreadUtil.performOnMainThread(() -> {
                    m_interstitialAd.loadAd();
                }, delayMillis);

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                m_interstitialAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdDisplayFailed");
            }
        };

        m_interstitialAd.setListener(maxAdListener);
    }

    public void loadInterstitial() {
        switch (m_loadInterstitialStatus) {
            case ADLOAD_NONE:
                MengineActivity activity = this.getActivity();

                final String AppLovin_AmazonInterstitialSlotId = activity.getConfigValue("AppLovin", "AmazonInterstitialSlotId", "");

                if (AppLovin_AmazonInterstitialSlotId.isEmpty() == false) {
                    m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_AMAZON;

                    DTBAdRequest adLoader = new DTBAdRequest();
                    adLoader.setSizes(new DTBAdSize.DTBInterstitialAdSize(AppLovin_AmazonInterstitialSlotId));
                    adLoader.loadAd(new DTBAdCallback() {
                        @Override
                        public void onSuccess(@NonNull final DTBAdResponse dtbAdResponse) {
                            // 'interstitialAd' is your instance of MaxInterstitialAd
                            m_interstitialAd.setLocalExtraParameter("amazon_ad_response", dtbAdResponse);
                            m_interstitialAd.loadAd();

                            m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_READY;
                        }

                        @Override
                        public void onFailure(@NonNull final AdError adError) {
                            // 'interstitialAd' is your instance of MaxInterstitialAd
                            m_interstitialAd.setLocalExtraParameter("amazon_ad_error", adError);
                            m_interstitialAd.loadAd();

                            m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_READY;
                        }
                    });
                } else {
                    m_interstitialAd.loadAd();

                    m_loadInterstitialStatus = ELoadAdStatus.ADLOAD_READY;
                }
                break;
            case ADLOAD_AMAZON:
                break;
            case ADLOAD_READY:
                m_interstitialAd.loadAd();
                break;
        }
    }

    public void showInterstitial() {
        if (m_interstitialAd.isReady()) {
            m_interstitialAd.showAd();
        }
    }

    public void initRewarded() throws Exception {
        MengineActivity activity = this.getActivity();

        String AppLovin_RewardedAdUnitId = activity.getConfigValue("AppLovin", "RewardedAdUnitId", "");

        if (AppLovin_RewardedAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] RewardedAdUnitId");
        }

        m_rewardedAd = MaxRewardedAd.getInstance(AppLovin_RewardedAdUnitId, activity);

        MaxRewardedAdListener maxRewardedAdListener = new MaxRewardedAdListener() {
            @Override
            public void onRewardedVideoStarted(MaxAd ad) {
                m_retryAttemptRewarded = 0;

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnRewardedVideoStarted");
            }

            @Override
            public void onRewardedVideoCompleted(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnRewardedVideoCompleted");
            }

            @Override
            public void onUserRewarded(MaxAd ad, MaxReward reward) {
                String label = reward.getLabel();
                int amount = reward.getAmount();

                MengineAppLovinPlugin.this.logInfo("rewarded %s [%d]"
                        , label
                        , amount
                );

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnUserRewarded", label, amount);
            }

            @Override
            public void onAdLoaded(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                m_rewardedAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                m_retryAttemptRewarded++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptRewarded)));

                ThreadUtil.performOnMainThread(() -> {
                    m_rewardedAd.loadAd();
                }, delayMillis);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                m_rewardedAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdDisplayFailed");
            }
        };

        m_rewardedAd.setListener(maxRewardedAdListener);
    }

    public void loadRewarded() {
        switch (m_loadRewardedStatus) {
            case ADLOAD_NONE:
                MengineActivity activity = this.getActivity();

                final String AppLovin_AmazonVideoRewardedSlotId = activity.getConfigValue("AppLovin", "AmazonVideoRewardedSlotId", "");

                if (AppLovin_AmazonVideoRewardedSlotId.isEmpty() == false) {
                    m_loadRewardedStatus = ELoadAdStatus.ADLOAD_AMAZON;

                    DTBAdRequest loader = new DTBAdRequest();
                    // Switch video player width and height values(320, 480) depending on device orientation

                    int width = activity.getWindow().getDecorView().getWidth();
                    int height = activity.getWindow().getDecorView().getHeight();

                    loader.setSizes(new DTBAdSize.DTBVideo(width, height, AppLovin_AmazonVideoRewardedSlotId));
                    loader.loadAd(new DTBAdCallback() {
                        @Override
                        public void onSuccess(@NonNull final DTBAdResponse dtbAdResponse) {
                            // 'rewardedAd' is your instance of MaxRewardedAd
                            m_rewardedAd.setLocalExtraParameter("amazon_ad_response", dtbAdResponse);
                            m_rewardedAd.loadAd();

                            m_loadRewardedStatus = ELoadAdStatus.ADLOAD_READY;
                        }

                        @Override
                        public void onFailure(@NonNull final AdError adError) {
                            // 'rewardedAd' is your instance of MaxRewardedAd
                            m_rewardedAd.setLocalExtraParameter("amazon_ad_error", adError);
                            m_rewardedAd.loadAd();

                            m_loadRewardedStatus = ELoadAdStatus.ADLOAD_READY;
                        }
                    });
                } else {
                    m_rewardedAd.loadAd();

                    m_loadRewardedStatus = ELoadAdStatus.ADLOAD_READY;
                }
                break;
            case ADLOAD_AMAZON:
                break;
            case ADLOAD_READY:
                m_rewardedAd.loadAd();
                break;
        }
    }

    public void showRewarded() {
        if (m_rewardedAd.isReady()) {
            m_rewardedAd.showAd();
        }
    }

    public void showMediationDebugger() {
        MengineActivity activity = this.getActivity();
        final Context context = activity.getBaseContext();

        AppLovinSdk.getInstance(context).showMediationDebugger();
    }
}
