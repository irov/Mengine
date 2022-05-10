package org.Mengine.Plugin.Applovin;

import android.content.Context;
import android.os.Bundle;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxReward;
import com.applovin.mediation.MaxRewardedAdListener;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.ThreadUtil;

import java.util.concurrent.TimeUnit;

public class MengineApplovinPlugin extends MenginePlugin {
    /**
     * - onMengineApplovinPluginOnSdkInitialized - вызов после успешной инициализации (после можно загружать рекламу)
     * <p>
     * Interstitial
     * void initInterstitial(String interstitial_ad_unit_id)
     * void loadInterstitial()
     * void showInterstitial()
     * - onMengineApplovinInterstitialOnAdLoaded
     * - onMengineApplovinInterstitialOnAdDisplayed
     * - onMengineApplovinInterstitialOnAdHidden
     * - onMengineApplovinInterstitialOnAdClicked
     * - onMengineApplovinInterstitialOnAdLoadFailed
     * - onMengineApplovinInterstitialOnAdDisplayFailed
     * <p>
     * установка Rewarded
     * void initRewarded(String rewarded_ad_unit_id)
     * void loadRewarded()
     * void showRewarded()
     * - onMengineApplovinRewardedOnRewardedVideoStarted
     * - onMengineApplovinRewardedOnRewardedVideoCompleted
     * - onMengineApplovinRewardedOnUserRewarded
     * - onMengineApplovinRewardedOnAdLoaded
     * - onMengineApplovinRewardedOnAdDisplayed
     * - onMengineApplovinRewardedOnAdHidden
     * - onMengineApplovinRewardedOnAdClicked
     * - onMengineApplovinRewardedOnAdLoadFailed
     * - onMengineApplovinRewardedOnAdDisplayFailed
     */

    private MaxInterstitialAd m_interstitialAd;
    private int m_retryAttemptInterstitial;

    private MaxRewardedAd m_rewardedAd;
    private int m_retryAttemptRewarded;

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("Applovin");
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        final Context context = activity.getBaseContext();

        AppLovinSdk.getInstance(context).setMediationProvider("max");
        AppLovinSdk.initializeSdk(context, new AppLovinSdk.SdkInitializationListener() {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration) {
                MengineApplovinPlugin.this.log("AppLovinSdk initialized: consent dialog [%s] country [%s]"
                    , configuration.getConsentDialogState().toString()
                    , configuration.getCountryCode()
                );

                //ToDo
                //MengineApplovinPlugin.this.pythonCall("onMengineApplovinPluginOnSdkInitialized");
            }
        });
    }

    public void initInterstitial(String interstitial_ad_unit_id) {
        m_interstitialAd = new MaxInterstitialAd(interstitial_ad_unit_id, getActivity());

        MaxAdListener maxAdListener = new MaxAdListener() {
            @Override
            public void onAdLoaded(MaxAd ad) {
                m_retryAttemptInterstitial = 0;

                MengineApplovinPlugin.this.pythonCall("onMengineApplovinInterstitialOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onMengineApplovinInterstitialOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                m_interstitialAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onMengineApplovinInterstitialOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onMengineApplovinInterstitialOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                m_retryAttemptInterstitial++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptInterstitial)));

                ThreadUtil.performOnMainThread(() -> {
                    m_interstitialAd.loadAd();
                }, delayMillis);

                MengineApplovinPlugin.this.pythonCall("onMengineApplovinInterstitialOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                m_interstitialAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onMengineApplovinInterstitialOnAdDisplayFailed");
            }
        };

        m_interstitialAd.setListener(maxAdListener);
    }

    public void initRewarded(String rewarded_ad_unit_id) {
        m_rewardedAd = MaxRewardedAd.getInstance(rewarded_ad_unit_id, getActivity());

        MaxRewardedAdListener maxRewardedAdListener = new MaxRewardedAdListener() {
            @Override
            public void onRewardedVideoStarted(MaxAd ad) {
                m_retryAttemptRewarded = 0;

                MengineApplovinPlugin.this.pythonCall("onMengineApplovinRewardedOnRewardedVideoStarted");
            }

            @Override
            public void onRewardedVideoCompleted(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onMengineApplovinRewardedOnRewardedVideoCompleted");
            }

            @Override
            public void onUserRewarded(MaxAd ad, MaxReward reward) {
                String label = reward.getLabel();
                int amount = reward.getAmount();

                MengineApplovinPlugin.this.info("rewarded %s [%d]"
                    , label
                    , amount
                );

                MengineApplovinPlugin.this.pythonCall("onMengineApplovinRewardedOnUserRewarded", label, amount);
            }

            @Override
            public void onAdLoaded(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onMengineApplovinRewardedOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onMengineApplovinRewardedOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                m_rewardedAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onMengineApplovinRewardedOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onMengineApplovinRewardedOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                m_retryAttemptRewarded++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptRewarded)));

                ThreadUtil.performOnMainThread(() -> {
                    m_rewardedAd.loadAd();
                }, delayMillis);

                MengineApplovinPlugin.this.pythonCall("onMengineApplovinRewardedOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                m_rewardedAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onMengineApplovinRewardedOnAdDisplayFailed");
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
}
