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

    static native void AndroidNativeUnity_onAdLoaded();

    static native void AndroidNativeUnity_onAdFailedToLoad(int errorCode);

    static native void AndroidNativeUnity_onAdOpened();

    static native void AndroidNativeUnity_onAdLeftApplication();

    static native void AndroidNativeUnity_onAdClosed();

    static native void AndroidNativeUnity_onRewardedVideoAdLoaded();

    static native void AndroidNativeUnity_onRewardedVideoAdOpened();

    static native void AndroidNativeUnity_onRewardedVideoStarted();

    static native void AndroidNativeUnity_onRewardedVideoAdClosed();

    static native void AndroidNativeUnity_onRewarded(String rewardType, int rewardAmount);

    static native void AndroidNativeUnity_onRewardedVideoAdLeftApplication();

    static native void AndroidNativeUnity_onRewardedVideoAdFailedToLoad(int errorCode);

    static native void AndroidNativeUnity_onRewardedVideoCompleted();

    public AdMobInteractionLayer(Activity activity) {
        MobileAds.initialize(activity, ADMOBAPPID);

        _interstitialAd = new InterstitialAd(activity);
        _interstitialAd.setAdUnitId(ADUNITID);
        _interstitialAd.setAdListener(new AdListener() {
            @Override
            public void onAdLoaded() {
                AndroidNativeUnity_onAdLoaded();
            }

            @Override
            public void onAdFailedToLoad(int errorCode) {
                AndroidNativeUnity_onAdFailedToLoad(errorCode);
            }

            @Override
            public void onAdOpened() {
                AndroidNativeUnity_onAdOpened();
            }

            @Override
            public void onAdLeftApplication() {
                AndroidNativeUnity_onAdLeftApplication();
            }

            @Override
            public void onAdClosed() {
                AndroidNativeUnity_onAdClosed();
            }
        });

        _rewardedVideoAd = MobileAds.getRewardedVideoAdInstance(activity);
        _rewardedVideoAd.setRewardedVideoAdListener(new RewardedVideoAdListener() {
            @Override
            public void onRewardedVideoAdLoaded() {
                AndroidNativeUnity_onRewardedVideoAdLoaded();
            }

            @Override
            public void onRewardedVideoAdOpened() {
                AndroidNativeUnity_onRewardedVideoAdOpened();
            }

            @Override
            public void onRewardedVideoStarted() {
                AndroidNativeUnity_onRewardedVideoStarted();
            }

            @Override
            public void onRewardedVideoAdClosed() {
                AndroidNativeUnity_onRewardedVideoAdClosed();
            }

            @Override
            public void onRewarded(RewardItem rewardItem) {
                if(rewardItem != null) {
                    AndroidNativeUnity_onRewarded(rewardItem.getType(), rewardItem.getAmount());
                } else {
                    AndroidNativeUnity_onRewarded("", 0);
                }
            }

            @Override
            public void onRewardedVideoAdLeftApplication() {
                AndroidNativeUnity_onRewardedVideoAdLeftApplication();
            }

            @Override
            public void onRewardedVideoAdFailedToLoad(int errorCode) {
                AndroidNativeUnity_onRewardedVideoAdFailedToLoad(errorCode);
            }

            @Override
            public void onRewardedVideoCompleted() {
                AndroidNativeUnity_onRewardedVideoCompleted();
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
