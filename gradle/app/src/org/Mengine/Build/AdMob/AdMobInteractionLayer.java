package org.Mengine.Build.AdMob;

import android.app.Activity;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.InterstitialAd;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.reward.RewardItem;
import com.google.android.gms.ads.reward.RewardedVideoAd;
import com.google.android.gms.ads.reward.RewardedVideoAdListener;

/**
 * Created by sweatcoin7 on 7/8/18.
 */

public class AdMobInteractionLayer {

    private static final String TAG = "AdMob";

    private static final String ADMOBAPPID = "ca-app-pub-3940256099942544~3347511713";
    private static final String ADUNITID = "ca-app-pub-3940256099942544/1033173712";

    private InterstitialAd _interstitialAd;
    private RewardedVideoAd _rewardedVideoAd;

    public AdMobInteractionLayer(Activity activity) {
        MobileAds.initialize(activity, ADMOBAPPID);

        _interstitialAd = new InterstitialAd(activity);
        _interstitialAd.setAdUnitId(ADUNITID);
        _interstitialAd.setAdListener(new AdListener() {
            @Override
            public void onAdLoaded() {
                // Code to be executed when an ad finishes loading.
            }

            @Override
            public void onAdFailedToLoad(int errorCode) {
                // Code to be executed when an ad request fails.
            }

            @Override
            public void onAdOpened() {
                // Code to be executed when the ad is displayed.
            }

            @Override
            public void onAdLeftApplication() {
                // Code to be executed when the user has left the app.
            }

            @Override
            public void onAdClosed() {
                // Code to be executed when when the interstitial ad is closed.
            }
        });

        _rewardedVideoAd = MobileAds.getRewardedVideoAdInstance(activity);
        _rewardedVideoAd.setRewardedVideoAdListener(new RewardedVideoAdListener() {
            @Override
            public void onRewardedVideoAdLoaded() {

            }

            @Override
            public void onRewardedVideoAdOpened() {

            }

            @Override
            public void onRewardedVideoStarted() {

            }

            @Override
            public void onRewardedVideoAdClosed() {

            }

            @Override
            public void onRewarded(RewardItem rewardItem) {

            }

            @Override
            public void onRewardedVideoAdLeftApplication() {

            }

            @Override
            public void onRewardedVideoAdFailedToLoad(int errorCode) {

            }

            @Override
            public void onRewardedVideoCompleted() {

            }
        });
    }

    public void setupInterstitialAd() {
        _interstitialAd.loadAd(new AdRequest.Builder().build());
    }

    public void showInterstitialAd() {
        if (!_interstitialAd.isLoaded()) {
            return;
        }
        _interstitialAd.show();
    }

    public void setupRewardedVideoAd() {
        _rewardedVideoAd.loadAd(ADUNITID,
                new AdRequest.Builder().build());
    }

    public void showRewardedVideoAd() {
        if (!_rewardedVideoAd.isLoaded()) {
            return;
        }
        _rewardedVideoAd.show();
    }
}
