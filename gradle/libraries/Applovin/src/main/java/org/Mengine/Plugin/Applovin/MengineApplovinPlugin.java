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

import org.Mengine.Build.MengineActivity;
import org.Mengine.Build.MenginePlugin;
import org.Mengine.Build.R;
import org.Mengine.Build.ThreadUtil;

import java.util.concurrent.TimeUnit;

public class MengineApplovinPlugin extends MenginePlugin {

    //каллебки для всех методов в нутри рекламы + OnDestroy
    /**
     * - onMengineApplovinPluginOnSdkInitialized - вызов после успешной инициализации (после можно загружать рекламу)
     * <p>
     * Interstitial
     * void initInterstitial(String interstitial_ad_unit_id)
     * void loadInterstitial()
     * void showInterstitial()
     * <p>
     * установка Rewarded
     * void initRewarded(String rewarded_ad_unit_id)
     * void loadRewarded()
     * void showRewarded()
     * - onMengineApplovinPluginOnUserRewarded
     */

    MaxInterstitialAd _interstitialAd;
    private int _retryAttemptInterstitial;

    private MaxRewardedAd _rewardedAd;
    private int _retryAttemptRewarded;

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("MengineApplovinPlugin");
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        super.onCreate(activity, savedInstanceState);

        final Context context = activity.getBaseContext();

        AppLovinSdk.getInstance(context).setMediationProvider("max");
        AppLovinSdk.initializeSdk(context, new AppLovinSdk.SdkInitializationListener() {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration) {
                MengineApplovinPlugin.this.pythonCall("onMengineApplovinPluginOnSdkInitialized");
            }
        });

    }

    @Override
    public void onDestroy(MengineActivity activity) {
        super.onDestroy(activity);
    }

    void initInterstitial(String interstitial_ad_unit_id) {
        //May be
        // ThreadUtil.performOnMainThread(() -> {
        _interstitialAd = new MaxInterstitialAd(interstitial_ad_unit_id, getActivity());
        _interstitialAd.setListener(_maxAdListener);
        //});
    }

    void initRewarded(String rewarded_ad_unit_id) {
        _rewardedAd = MaxRewardedAd.getInstance(rewarded_ad_unit_id, getActivity());
        _rewardedAd.setListener(_maxRewardedAdListener);
    }

    void loadInterstitial() {
        _interstitialAd.loadAd();
    }

    void loadRewarded() {
        _rewardedAd.loadAd();
    }

    MaxAdListener _maxAdListener = new MaxAdListener() {
        @Override
        public void onAdLoaded(MaxAd ad) {
            _retryAttemptInterstitial = 0;
        }

        @Override
        public void onAdDisplayed(MaxAd ad) {

        }

        @Override
        public void onAdHidden(MaxAd ad) {
            _interstitialAd.loadAd();
        }

        @Override
        public void onAdClicked(MaxAd ad) {
            MengineApplovinPlugin.this.pythonCall("onMengineApplovinInterstitialOnAdClicked");
        }

        @Override
        public void onAdLoadFailed(String adUnitId, MaxError error) {
            _retryAttemptInterstitial++;
            long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, _retryAttemptInterstitial)));

            ThreadUtil.performOnMainThread(() -> {
                _interstitialAd.loadAd();
            }, delayMillis);
        }

        @Override
        public void onAdDisplayFailed(MaxAd ad, MaxError error) {
            _interstitialAd.loadAd();
        }
    };

    MaxRewardedAdListener _maxRewardedAdListener = new MaxRewardedAdListener() {
        @Override
        public void onRewardedVideoStarted(MaxAd ad) {
            _retryAttemptRewarded = 0;
        }

        @Override
        public void onRewardedVideoCompleted(MaxAd ad) {

        }

        @Override
        public void onUserRewarded(MaxAd ad, MaxReward reward) {
            log("rewarded received %i", reward.getAmount());
            MengineApplovinPlugin.this.pythonCall("onMengineApplovinOnUserRewarded", reward.getAmount());
        }

        @Override
        public void onAdLoaded(MaxAd ad) {

        }

        @Override
        public void onAdDisplayed(MaxAd ad) {

        }

        @Override
        public void onAdHidden(MaxAd ad) {
            _rewardedAd.loadAd();
        }

        @Override
        public void onAdClicked(MaxAd ad) {

        }

        @Override
        public void onAdLoadFailed(String adUnitId, MaxError error) {

            _retryAttemptRewarded++;
            long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, _retryAttemptRewarded)));

            ThreadUtil.performOnMainThread(() -> {
                _rewardedAd.loadAd();
            }, delayMillis);
        }

        @Override
        public void onAdDisplayFailed(MaxAd ad, MaxError error) {
            _rewardedAd.loadAd();
        }
    };

    void showInterstitial() {
        if (_interstitialAd.isReady()) {
            _interstitialAd.showAd();
        }
    }

    void showRewarded() {
        if (_rewardedAd.isReady()) {
            _rewardedAd.showAd();
        }
    }

}
