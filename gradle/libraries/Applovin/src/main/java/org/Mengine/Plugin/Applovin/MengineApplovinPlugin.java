package org.Mengine.Plugin.Applovin;

import android.content.Context;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.applovin.mediation.MaxAd;
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

public class MengineApplovinPlugin extends MenginePlugin {
    /**
     * - onApplovinPluginOnSdkInitialized - вызов после успешной инициализации (после можно загружать рекламу)
     * <p>
     * Interstitial
     * void initInterstitial(String interstitial_ad_unit_id)
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
     * void initRewarded(String rewarded_ad_unit_id)
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
     *  - onApplovinBannerOnAdDisplayed
     *  - onApplovinBannerOnAdHidden
     *  - onApplovinBannerOnAdClicked
     *  - onApplovinBannerOnAdLoadFailed
     *  - onApplovinBannerOnAdDisplayFailed
     *  - onApplovinBannerOnAdExpanded
     *  - onApplovinBannerOnAdCollapsed
     */

    private MaxInterstitialAd m_interstitialAd;
    private int m_retryAttemptInterstitial;

    private MaxRewardedAd m_rewardedAd;
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

        AppLovinSdk.initializeSdk(context, new AppLovinSdk.SdkInitializationListener() {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration) {
                MengineApplovinPlugin.this.logInfo("AppLovinSdk initialized: consent dialog [%s] country [%s]"
                        , configuration.getConsentDialogState().toString()
                        , configuration.getCountryCode()
                );

                MengineApplovinPlugin.this.pythonCall("onApplovinPluginOnSdkInitialized");
            }
        });
    }

    public void initBanner() throws Exception {
        MengineActivity activity = this.getActivity();

        String Applovin_BannerAdUnitId = activity.getConfigValue("AppLovin", "BannerAdUnitId", "");

        if (Applovin_BannerAdUnitId == "") {
            throw new Exception("Need to add config value for [AppLovin] BannerAdUnitId");
        }

        m_adView = new MaxAdView(Applovin_BannerAdUnitId, activity);
        MaxAdViewAdListener maxAdViewAdListener = new MaxAdViewAdListener() {

            @Override
            public void onAdLoaded(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinBannerOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinBannerOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinBannerOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinBannerOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                MengineApplovinPlugin.this.pythonCall("onApplovinBannerOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                MengineApplovinPlugin.this.pythonCall("onApplovinBannerOnAdDisplayFailed");
            }

            @Override
            public void onAdExpanded(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinBannerOnAdExpanded");
            }

            @Override
            public void onAdCollapsed(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinBannerOnAdCollapsed");
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

        // Load the ad
        m_adView.loadAd();

        bannerVisible(false);
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

        String Applovin_InterstitialAdUnitId = activity.getConfigValue("AppLovin", "InterstitialAdUnitId", "");

        if (Applovin_InterstitialAdUnitId == "") {
            throw new Exception("Need to add config value for [AppLovin] InterstitialAdUnitId");
        }

        m_interstitialAd = new MaxInterstitialAd(Applovin_InterstitialAdUnitId, activity);

        MaxAdListener maxAdListener = new MaxAdListener() {
            @Override
            public void onAdLoaded(MaxAd ad) {
                m_retryAttemptInterstitial = 0;

                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                m_interstitialAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                m_retryAttemptInterstitial++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptInterstitial)));

                ThreadUtil.performOnMainThread(() -> {
                    m_interstitialAd.loadAd();
                }, delayMillis);

                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                m_interstitialAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdDisplayFailed");
            }
        };

        m_interstitialAd.setListener(maxAdListener);
    }

    public void initRewarded() throws Exception {
        MengineActivity activity = this.getActivity();

        String Applovin_RewardedAdUnitId = activity.getConfigValue("AppLovin", "RewardedAdUnitId", "");

        if (Applovin_RewardedAdUnitId == "") {
            throw new Exception("Need to add config value for [AppLovin] RewardedAdUnitId");
        }

        m_rewardedAd = MaxRewardedAd.getInstance(Applovin_RewardedAdUnitId, activity);

        MaxRewardedAdListener maxRewardedAdListener = new MaxRewardedAdListener() {
            @Override
            public void onRewardedVideoStarted(MaxAd ad) {
                m_retryAttemptRewarded = 0;

                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnRewardedVideoStarted");
            }

            @Override
            public void onRewardedVideoCompleted(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnRewardedVideoCompleted");
            }

            @Override
            public void onUserRewarded(MaxAd ad, MaxReward reward) {
                String label = reward.getLabel();
                int amount = reward.getAmount();

                MengineApplovinPlugin.this.logInfo("rewarded %s [%d]"
                        , label
                        , amount
                );

                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnUserRewarded", label, amount);
            }

            @Override
            public void onAdLoaded(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                m_rewardedAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                m_retryAttemptRewarded++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptRewarded)));

                ThreadUtil.performOnMainThread(() -> {
                    m_rewardedAd.loadAd();
                }, delayMillis);

                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                m_rewardedAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdDisplayFailed");
            }
        };

        m_rewardedAd.setListener(maxRewardedAdListener);
    }

    public void loadInterstitial() {
        m_interstitialAd.loadAd();
    }

    public void loadRewarded() {
        m_rewardedAd.loadAd();
    }

    public void showInterstitial() {
        if (m_interstitialAd.isReady()) {
            m_interstitialAd.showAd();
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
