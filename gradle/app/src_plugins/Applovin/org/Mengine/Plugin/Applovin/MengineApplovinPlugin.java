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
import com.google.android.datatransport.runtime.logging.Logging;

import org.Mengine.Build.MengineActivity;
import org.Mengine.Build.MenginePlugin;
import org.Mengine.Build.R;
import org.Mengine.Build.ThreadUtil;

import java.util.concurrent.TimeUnit;

public class MengineApplovinPlugin extends MenginePlugin {

    MaxInterstitialAd _interstitialAd;
    private int _retryAttemptInterstitial;

    private MaxRewardedAd rewardedAd;
    private int retryAttempt;

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
                startLoading();
            }
        });


        _interstitialAd = new MaxInterstitialAd(activity.getString(R.string.applovin_interstitial_ad_unit_id), activity);
        _interstitialAd.setListener(_maxAdListener);

        rewardedAd = MaxRewardedAd.getInstance(activity.getString(R.string.applovin_rewarded_ad_unit_id), activity);
        rewardedAd.setListener(_maxRewardedAdListener);

    }

    private boolean _hasStartingLoaded = false;

    public void startLoading() {
        //делать ли загрузку с проверкой после старта приложения
        //чтоб загрузка была после инициализцзации всего
        if (_hasStartingLoaded) return;
        _hasStartingLoaded = true;
        ThreadUtil.performOnMainThread(() -> {
            _interstitialAd.loadAd();
            rewardedAd.loadAd();
        }, 4000);
    }

    //    нужно ли загружать рекламу по запросу
//    чтоб небыло долгой загрузки приложения
//    сейчас интестишалы грузятся один раз - и далее по каллбеку в нутри applovin

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
            retryAttempt = 0;
        }

        @Override
        public void onRewardedVideoCompleted(MaxAd ad) {

        }

        @Override
        public void onUserRewarded(MaxAd ad, MaxReward reward) {
            log("rewarded received %i", reward.getAmount());
        }

        @Override
        public void onAdLoaded(MaxAd ad) {

        }

        @Override
        public void onAdDisplayed(MaxAd ad) {

        }

        @Override
        public void onAdHidden(MaxAd ad) {
            rewardedAd.loadAd();
        }

        @Override
        public void onAdClicked(MaxAd ad) {

        }

        @Override
        public void onAdLoadFailed(String adUnitId, MaxError error) {

            retryAttempt++;
            long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, retryAttempt)));

            ThreadUtil.performOnMainThread(() -> {
                rewardedAd.loadAd();
            }, delayMillis);
        }

        @Override
        public void onAdDisplayFailed(MaxAd ad, MaxError error) {
            rewardedAd.loadAd();
        }
    };

    void showInterstitial() {
        if (_interstitialAd.isReady()) {
            _interstitialAd.showAd();
        }
    }

    void showRewarded() {
        if (rewardedAd.isReady()) {
            rewardedAd.showAd();
        }
    }

}
