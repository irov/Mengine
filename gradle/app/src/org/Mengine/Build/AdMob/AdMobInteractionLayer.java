package org.Mengine.Build.AdMob;

import android.app.Activity;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.InterstitialAd;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.reward.RewardItem;
import com.google.android.gms.ads.reward.RewardedVideoAd;
import com.google.android.gms.ads.reward.RewardedVideoAdListener;

import org.Mengine.Build.ThreadUtil;

/**
 * Created by sweatcoin7 on 7/8/18.
 */

public class AdMobInteractionLayer {

    private static final String TAG = "AdMob";

    private static final String ADMOBAPPID = "ca-app-pub-3940256099942544~3347511713";
    private static final String ADUNITID = "ca-app-pub-3940256099942544/1033173712";

    private InterstitialAd _interstitialAd;
    private RewardedVideoAd _rewardedVideoAd;

    static native void AndroidNativeAdMob_onAdLoaded();

    static native void AndroidNativeAdMob_onAdFailedToLoad(int errorCode);

    static native void AndroidNativeAdMob_onAdOpened();

    static native void AndroidNativeAdMob_onAdLeftApplication();

    static native void AndroidNativeAdMob_onAdClosed();

    static native void AndroidNativeAdMob_onRewardedVideoAdLoaded();

    static native void AndroidNativeAdMob_onRewardedVideoAdOpened();

    static native void AndroidNativeAdMob_onRewardedVideoStarted();

    static native void AndroidNativeAdMob_onRewardedVideoAdClosed();

    static native void AndroidNativeAdMob_onRewarded(String rewardType, int rewardAmount);

    static native void AndroidNativeAdMob_onRewardedVideoAdLeftApplication();

    static native void AndroidNativeAdMob_onRewardedVideoAdFailedToLoad(int errorCode);

    static native void AndroidNativeAdMob_onRewardedVideoCompleted();

    public AdMobInteractionLayer(Activity activity) {
        MobileAds.initialize(activity, ADMOBAPPID);

        _interstitialAd = new InterstitialAd(activity);
        _interstitialAd.setAdUnitId(ADUNITID);
        _interstitialAd.setAdListener(new AdListener() {
            @Override
            public void onAdLoaded() {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onAdLoaded();
                    }
                });
            }

            @Override
            public void onAdFailedToLoad(final int errorCode) {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onAdFailedToLoad(errorCode);
                    }
                });
            }

            @Override
            public void onAdOpened() {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onAdOpened();
                    }
                });
            }

            @Override
            public void onAdLeftApplication() {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onAdLeftApplication();
                    }
                });
            }

            @Override
            public void onAdClosed() {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onAdClosed();
                    }
                });
            }
        });

        _rewardedVideoAd = MobileAds.getRewardedVideoAdInstance(activity);
        _rewardedVideoAd.setRewardedVideoAdListener(new RewardedVideoAdListener() {
            @Override
            public void onRewardedVideoAdLoaded() {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onRewardedVideoAdLoaded();
                    }
                });
            }

            @Override
            public void onRewardedVideoAdOpened() {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onRewardedVideoAdOpened();
                    }
                });
            }

            @Override
            public void onRewardedVideoStarted() {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onRewardedVideoStarted();
                    }
                });
            }

            @Override
            public void onRewardedVideoAdClosed() {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onRewardedVideoAdClosed();
                    }
                });
            }

            @Override
            public void onRewarded(final RewardItem rewardItem) {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        if (rewardItem != null) {
                            AndroidNativeAdMob_onRewarded(rewardItem.getType(), rewardItem.getAmount());
                        } else {
                            AndroidNativeAdMob_onRewarded("", 0);
                        }
                    }
                });
            }

            @Override
            public void onRewardedVideoAdLeftApplication() {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onRewardedVideoAdLeftApplication();
                    }
                });
            }

            @Override
            public void onRewardedVideoAdFailedToLoad(final int errorCode) {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onRewardedVideoAdFailedToLoad(errorCode);
                    }
                });
            }

            @Override
            public void onRewardedVideoCompleted() {
                ThreadUtil.performOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        AndroidNativeAdMob_onRewardedVideoCompleted();
                    }
                });
            }
        });
    }

    public void setupInterstitialAd() {
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                _interstitialAd.loadAd(new AdRequest.Builder().build());
            }
        });
    }

    public void showInterstitialAd() {
        if (!_interstitialAd.isLoaded()) {
            return;
        }
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                _interstitialAd.show();
            }
        });
    }

    public void setupRewardedVideoAd() {
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                _rewardedVideoAd.loadAd(ADUNITID,
                        new AdRequest.Builder().build());
            }
        });
    }

    public void showRewardedVideoAd() {
        if (!_rewardedVideoAd.isLoaded()) {
            return;
        }
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                _rewardedVideoAd.show();
            }
        });
    }
}
