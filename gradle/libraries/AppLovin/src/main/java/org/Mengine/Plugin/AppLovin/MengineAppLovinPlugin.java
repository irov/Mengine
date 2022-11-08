package org.Mengine.Plugin.AppLovin;

import org.Mengine.Plugin.AppLovin.MengineAppLovinMediationInterface;

import org.Mengine.Base.MengineUtils;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineUtils;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxAdViewAdListener;
import com.applovin.mediation.MaxAdWaterfallInfo;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxNetworkResponseInfo;
import com.applovin.mediation.MaxReward;
import com.applovin.mediation.MaxRewardedAdListener;
import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;

import com.applovin.sdk.AppLovinAdSize;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;
import com.applovin.sdk.AppLovinSdkUtils;

import android.content.Context;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.FrameLayout;
import androidx.annotation.NonNull;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.concurrent.TimeUnit;

public class MengineAppLovinPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "AppLovin";
    public static boolean PLUGIN_EMBEDDING = true;

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
     * <p>
     * void showMediationDebugger()
     */

    enum ELoadAdStatus {
        ADLOAD_NONE,
        ADLOAD_AMAZON,
        ADLOAD_READY
    }

    private MaxInterstitialAd m_interstitialAd;
    private int m_retryAttemptInterstitial;

    private MaxRewardedAd m_rewardedAd;
    private int m_retryAttemptRewarded;

    private MaxAdView m_adView;

    private MengineAppLovinMediationInterface m_mediationAmazon;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        if (m_mediationAmazon != null) {
            m_mediationAmazon = null;
        }

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

        m_mediationAmazon = this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinMediationAmazon", false);

        if (m_mediationAmazon != null) {
            try {
                m_mediationAmazon.initializeMediator(activity);
            } catch (Exception e) {

            }
        }

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);

        appLovinSdk.setMediationProvider("max");

        boolean OPTION_applovinverbose = activity.hasOption("applovinverbose");
        boolean AppLovin_VerboseLogging = activity.getConfigValueBoolean("AppLovinPlugin", "VerboseLogging", false);

        if( OPTION_applovinverbose == true || AppLovin_VerboseLogging == true ) {
            appLovinSdk.getSettings().setVerboseLogging(true);
        }

        AppLovinSdk.initializeSdk(context, new AppLovinSdk.SdkInitializationListener() {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration) {
                MengineAppLovinPlugin.this.logInfo("AppLovinSdk initialized: country [%s]"
                    , configuration.getCountryCode()
                );

                MengineAppLovinPlugin.this.pythonCall("onApplovinPluginOnSdkInitialized");
            }
        });

        boolean OPTION_applovindebugger = activity.hasOption("applovindebugger");

        if (OPTION_applovindebugger == true) {
            this.showMediationDebugger();
        }
    }

    public void initBanner() throws Exception {
        MengineActivity activity = this.getActivity();

        String AppLovin_BannerAdUnitId = activity.getConfigValue("AppLovinPlugin", "BannerAdUnitId", "");

        if (AppLovin_BannerAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] BannerAdUnitId");
        }

        m_adView = new MaxAdView(AppLovin_BannerAdUnitId, activity);

        MaxAdViewAdListener maxAdViewAdListener = new MaxAdViewAdListener() {
            @Override
            public void onAdLoaded(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd( "Banner","onAdLoaded", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Banner", "onAdDisplayed", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Banner", "onAdHidden", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Banner", "onAdClicked", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Banner", "onAdLoadFailed", error);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Banner", "onAdDisplayFailed", error);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdDisplayFailed");
            }

            @Override
            public void onAdExpanded(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Banner", "onAdExpanded", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdExpanded");
            }

            @Override
            public void onAdCollapsed(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Banner", "onAdCollapsed", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdCollapsed");
            }
        };

        m_adView.setListener(maxAdViewAdListener);

        int width = ViewGroup.LayoutParams.MATCH_PARENT;
        boolean tablet = AppLovinSdkUtils.isTablet(activity);
        AppLovinAdSize size = tablet ? AppLovinAdSize.LEADER : AppLovinAdSize.BANNER;
        int tablet_size_height = size.getHeight();

        int heightPx = AppLovinSdkUtils.dpToPx(activity, tablet_size_height);

        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(width, heightPx);
        m_adView.setLayoutParams(params);

        Window window = activity.getWindow();
        View view = window.getDecorView();
        int window_view_height = view.getHeight();

        m_adView.setTranslationY(window_view_height - heightPx);

        ViewGroup rootView = view.findViewById(android.R.id.content);
        rootView.addView(m_adView);

        if (m_mediationAmazon != null) {
            m_mediationAmazon.initializeMediatorBanner(activity, m_adView);
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

        String AppLovin_InterstitialAdUnitId = activity.getConfigValue("AppLovinPlugin", "InterstitialAdUnitId", "");

        if (AppLovin_InterstitialAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] InterstitialAdUnitId");
        }

        m_interstitialAd = new MaxInterstitialAd(AppLovin_InterstitialAdUnitId, activity);

        MaxAdListener maxAdListener = new MaxAdListener() {
            @Override
            public void onAdLoaded(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Interstitial", "onAdLoaded", ad);

                m_retryAttemptInterstitial = 0;

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Interstitial", "onAdDisplayed", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Interstitial", "onAdHidden", ad);

                m_interstitialAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Interstitial", "onAdClicked", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Interstitial", "onAdLoadFailed", error);

                m_retryAttemptInterstitial++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptInterstitial)));

                MengineUtils.performOnMainThread(() -> {
                    m_interstitialAd.loadAd();
                }, delayMillis);

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Interstitial", "onAdDisplayFailed", error);

                m_interstitialAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdDisplayFailed");
            }
        };

        m_interstitialAd.setListener(maxAdListener);
    }

    public void loadInterstitial() {
        MengineActivity activity = this.getActivity();

        if (m_mediationAmazon != null) {
            try {
                m_mediationAmazon.loadMediatorInterstitial(activity, m_interstitialAd);
            } catch (Exception e) {
            }
        } else {
            m_interstitialAd.loadAd();
        }
    }

    public void showInterstitial() {
        if (m_interstitialAd.isReady()) {
            m_interstitialAd.showAd();
        }
    }

    public void initRewarded() throws Exception {
        MengineActivity activity = this.getActivity();

        String AppLovin_RewardedAdUnitId = activity.getConfigValue("AppLovinPlugin", "RewardedAdUnitId", "");

        if (AppLovin_RewardedAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] RewardedAdUnitId");
        }

        m_rewardedAd = MaxRewardedAd.getInstance(AppLovin_RewardedAdUnitId, activity);

        MaxRewardedAdListener maxRewardedAdListener = new MaxRewardedAdListener() {
            @Override
            public void onRewardedVideoStarted(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onRewardedVideoStarted", ad);

                m_retryAttemptRewarded = 0;

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnRewardedVideoStarted");
            }

            @Override
            public void onRewardedVideoCompleted(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onRewardedVideoCompleted", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnRewardedVideoCompleted");
            }

            @Override
            public void onUserRewarded(MaxAd ad, MaxReward reward) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onUserRewarded", ad);

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
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onAdLoaded", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onAdDisplayed", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onAdHidden", ad);

                m_rewardedAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onAdClicked", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Rewarded", "onAdLoadFailed", error);

                m_retryAttemptRewarded++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptRewarded)));

                MengineUtils.performOnMainThread(() -> {
                    m_rewardedAd.loadAd();
                }, delayMillis);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Rewarded", "onAdDisplayFailed", error);

                m_rewardedAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdDisplayFailed");
            }
        };

        m_rewardedAd.setListener(maxRewardedAdListener);
    }

    public void loadRewarded() {
        MengineActivity activity = this.getActivity();

        if (m_mediationAmazon != null) {
            try {
                m_mediationAmazon.loadMediatorRewarded(activity, m_rewardedAd);
            } catch (Exception e) {
            }
        } else {
            m_rewardedAd.loadAd();
        }
    }

    public void showRewarded() {
        if (m_rewardedAd.isReady() == true) {
            m_rewardedAd.showAd();
        }
    }

    public void logMaxAd(String type, String callback, MaxAd ad) {
        this.logInfo( "AppLovin: type: " + type + " callback: " + callback );
        this.logInfo( "Waterfall Name: " + ad.getWaterfall().getName() + " and Test Name: " + ad.getWaterfall().getTestName() );
        this.logInfo( "Waterfall latency was: " + ad.getWaterfall().getLatencyMillis() + " milliseconds" );

        for (MaxNetworkResponseInfo networkResponse : ad.getWaterfall().getNetworkResponses()) {
            String waterfallInfoStr = "Network -> " + networkResponse.getMediatedNetwork() +
                    "\n...adLoadState: " + networkResponse.getAdLoadState() +
                    "\n...latency: " + networkResponse.getLatencyMillis() + " milliseconds" +
                    "\n...credentials: " + networkResponse.getCredentials();

            if (networkResponse.getError() != null) {
                waterfallInfoStr += "\n...error: " + networkResponse.getError();
            }

            this.logInfo(waterfallInfoStr);
        }
    }

    public void logMaxError(String type, String callback, MaxError error) {
        this.logInfo( "AppLovin: type: " + type + " callback: " + callback );

        MaxAdWaterfallInfo waterfall = error.getWaterfall();

        this.logMaxAdWaterfallInfo(waterfall);
    }

    public void logMaxAdWaterfallInfo(MaxAdWaterfallInfo waterfall) {
        this.logInfo( "Waterfall Name: " + waterfall.getName() + " and Test Name: " + waterfall.getTestName() );
        this.logInfo( "Waterfall latency was: " + waterfall.getLatencyMillis() + " milliseconds" );

        for (MaxNetworkResponseInfo networkResponse : waterfall.getNetworkResponses()) {
            this.logMaxNetworkResponseInfo(networkResponse);
        }
    }

    public void logMaxNetworkResponseInfo(MaxNetworkResponseInfo networkResponse) {
        String waterfallInfoStr = "Network -> " + networkResponse.getMediatedNetwork() +
                "\n...adLoadState: " + networkResponse.getAdLoadState() +
                "\n...latency: " + networkResponse.getLatencyMillis() + " milliseconds" +
                "\n...credentials: " + networkResponse.getCredentials();

        if (networkResponse.getError() != null) {
            waterfallInfoStr += "\n...error: " + networkResponse.getError();
        }

        this.logInfo(waterfallInfoStr);
    }

    public void showMediationDebugger() {
        MengineActivity activity = this.getActivity();
        final Context context = activity.getBaseContext();

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);
        appLovinSdk.showMediationDebugger();
    }
}
