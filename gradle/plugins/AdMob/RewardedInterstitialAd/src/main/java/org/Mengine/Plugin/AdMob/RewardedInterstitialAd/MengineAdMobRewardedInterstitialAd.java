package org.Mengine.Plugin.AdMob.RewardedInterstitialAd;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.ResponseInfo;
import com.google.android.gms.ads.rewardedinterstitial.RewardedInterstitialAd;
import com.google.android.gms.ads.rewardedinterstitial.RewardedInterstitialAdLoadCallback;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdResponseInterface;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineAnalyticsEventBuilderInterface;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MenginePlatformEventQueue;
import org.Mengine.Base.MengineTag;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobBase;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobPluginInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobRewardedInterstitialAdInterface;

import java.util.Map;

public class MengineAdMobRewardedInterstitialAd extends MengineAdMobBase implements MengineAdMobRewardedInterstitialAdInterface {
    public static final MengineTag TAG = MengineTag.of("MNGAdMobRewardedIntAd");

    private LoadCallback m_loadCallback;
    private volatile RewardedInterstitialAd m_rewardedInterstitialAd;

    private volatile boolean m_showing = false;
    private @NonNull String m_placement = "";

    public MengineAdMobRewardedInterstitialAd(@NonNull MengineAdService adService, @NonNull MengineAdMobPluginInterface plugin, @NonNull String adUnitId) {
        super(adService, plugin, MengineAdFormat.ADFORMAT_REWARDED_INTERSTITIAL, adUnitId);
    }

    protected MengineAnalyticsEventBuilderInterface buildRewardedInterstitialAdEvent(@Size(min = 1, max = 40) String event) {
        MengineAnalyticsEventBuilderInterface builder = this.buildAdEvent("mng_admob_rewarded_int_" + event);

        return builder;
    }

    protected void setRewardedInterstitialState(@NonNull String state) {
        this.setState("admob.rewarded_interstitial.state." + m_adUnitId, state);
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) {
        super.onActivityCreate(activity);

        m_loadCallback = new LoadCallback(this);

        this.log("create");

        this.setRewardedInterstitialState("init");

        this.loadAd();
    }

    @Override
    public void onActivityDestroy(@NonNull MengineActivity activity) {
        super.onActivityDestroy(activity);

        if (m_loadCallback != null) {
            m_loadCallback.destroy();
            m_loadCallback = null;
        }

        this.destroyRewardedInterstitialAd();
    }

    private void destroyRewardedInterstitialAd() {
        if (m_rewardedInterstitialAd != null) {
            m_rewardedInterstitialAd.setFullScreenContentCallback(null);
            m_rewardedInterstitialAd.setOnPaidEventListener(null);

            m_rewardedInterstitialAd = null;
        }

        if (m_showing == true) {
            MenginePlatformEventQueue.pushFreezeEvent(TAG.toString(), false);
        }

        m_showing = false;
        m_placement = "";
    }

    @Override
    public void loadAd() {
        if (m_plugin.hasOption("admob.rewarded_interstitial.disable") == true || m_plugin.hasOption("admob.ad.disable") == true) {
            return;
        }

        this.log("loadAd");

        this.increaseRequestId();

        this.buildRewardedInterstitialAdEvent("load")
            .log();

        this.setRewardedInterstitialState("load");

        MengineActivity activity = m_plugin.getMengineActivity();

        if (activity == null) {
            return;
        }

        try {
            AdRequest adRequest = new AdRequest.Builder().build();

            RewardedInterstitialAd.load(activity, m_adUnitId, adRequest, m_loadCallback);
        } catch (final Exception e) {
            this.logError("loadAd", e);

            this.buildRewardedInterstitialAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.setRewardedInterstitialState("load_exception");

            this.retryLoadAd();
        }
    }

    private void handleAdLoaded(@NonNull RewardedInterstitialAd rewardedAd) {
        MengineAdResponseInterface adResponse = m_adService.getAdResponse();

        this.destroyRewardedInterstitialAd();

        m_rewardedInterstitialAd = rewardedAd;

        m_rewardedInterstitialAd.setFullScreenContentCallback(new FullScreenContentCallback() {
            @Override
            public void onAdDismissedFullScreenContent() {
                m_rewardedInterstitialAd = null;

                MengineAdMobRewardedInterstitialAd.this.log("onAdDismissedFullScreenContent");

                MengineAdMobRewardedInterstitialAd.this.buildRewardedInterstitialAdEvent("dismissed")
                    .log();

                MengineAdMobRewardedInterstitialAd.this.setRewardedInterstitialState("dismissed");

                MenginePlatformEventQueue.pushFreezeEvent(TAG.toString(), false);

                m_showing = false;

                adResponse.onAdShowSuccess(MengineAdMediation.ADMEDIATION_ADMOB, MengineAdFormat.ADFORMAT_REWARDED_INTERSTITIAL, m_placement);

                MengineAdMobRewardedInterstitialAd.this.performOnMainThread(() -> {
                    MengineAdMobRewardedInterstitialAd.this.loadAd();
                });
            }

            @Override
            public void onAdFailedToShowFullScreenContent(@NonNull AdError adError) {
                m_rewardedInterstitialAd = null;

                MengineAdMobRewardedInterstitialAd.this.logAdError("onAdFailedToShowFullScreenContent", adError);

                int errorCode = adError.getCode();

                MengineAdMobRewardedInterstitialAd.this.buildRewardedInterstitialAdEvent("show_failed")
                    .addParameterJSON("error", MengineAdMobRewardedInterstitialAd.this.getAdErrorParams(adError))
                    .addParameterLong("error_code", errorCode)
                    .log();

                MengineAdMobRewardedInterstitialAd.this.setRewardedInterstitialState("show_failed." + errorCode);

                MenginePlatformEventQueue.pushFreezeEvent(TAG.toString(), false);

                m_showing = false;

                adResponse.onAdShowFailed(MengineAdMediation.ADMEDIATION_ADMOB, MengineAdFormat.ADFORMAT_REWARDED_INTERSTITIAL, m_placement, errorCode);

                MengineAdMobRewardedInterstitialAd.this.performOnMainThread(() -> {
                    MengineAdMobRewardedInterstitialAd.this.loadAd();
                });
            }

            @Override
            public void onAdShowedFullScreenContent() {
                MengineAdMobRewardedInterstitialAd.this.log("onAdShowedFullScreenContent");

                MengineAdMobRewardedInterstitialAd.this.buildRewardedInterstitialAdEvent("showed")
                    .log();

                MengineAdMobRewardedInterstitialAd.this.setRewardedInterstitialState("showed");

                MenginePlatformEventQueue.pushFreezeEvent(TAG.toString(), true);
            }

            @Override
            public void onAdClicked() {
                MengineAdMobRewardedInterstitialAd.this.log("onAdClicked");

                MengineAdMobRewardedInterstitialAd.this.buildRewardedInterstitialAdEvent("clicked")
                    .log();

                MengineAdMobRewardedInterstitialAd.this.setRewardedInterstitialState("clicked");
            }

            @Override
            public void onAdImpression() {
                MengineAdMobRewardedInterstitialAd.this.log("onAdImpression");

                MengineAdMobRewardedInterstitialAd.this.buildRewardedInterstitialAdEvent("impression")
                    .log();

                MengineAdMobRewardedInterstitialAd.this.setRewardedInterstitialState("impression");
            }
        });

        m_rewardedInterstitialAd.setOnPaidEventListener(adValue -> {
            MengineAdMobRewardedInterstitialAd.this.log("onPaidEvent");

            if (m_rewardedInterstitialAd != null) {
                ResponseInfo responseInfo = m_rewardedInterstitialAd.getResponseInfo();

                if (responseInfo != null) {
                    long valueMicros = adValue.getValueMicros();
                    double value = valueMicros / 1000000.0;

                    MengineAdMobRewardedInterstitialAd.this.revenuePaid(responseInfo, MengineAdFormat.ADFORMAT_REWARDED_INTERSTITIAL, m_placement, value);
                }
            }
        });

        this.log("onAdLoaded");

        this.buildRewardedInterstitialAdEvent("loaded")
            .log();

        this.setRewardedInterstitialState("loaded");

        m_requestAttempt = 0;
    }

    private void handleAdFailedToLoad(@NonNull LoadAdError loadAdError) {
        this.destroyRewardedInterstitialAd();

        this.logLoadAdError("onAdFailedToLoad", loadAdError);

        int errorCode = loadAdError.getCode();

        this.buildRewardedInterstitialAdEvent("load_failed")
            .addParameterJSON("error", this.getLoadAdErrorParams(loadAdError))
            .addParameterLong("error_code", errorCode)
            .log();

        this.setRewardedInterstitialState("load_failed." + errorCode);

        this.retryLoadAd();
    }

    public boolean canYouShowRewardedInterstitial(String placement) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_rewardedInterstitialAd != null;

        if (m_showing == true) {
            ready = false;
        }

        this.log("canYouShowRewardedInterstitial", Map.of("placement", placement, "ready", ready));

        if (ready == false) {
            this.buildRewardedInterstitialAdEvent("show")
                .addParameterString("placement", placement)
                .addParameterBoolean("ready", false)
                .log();

            return false;
        }

        return true;
    }

    public boolean showRewardedInterstitial(@NonNull MengineActivity activity, @NonNull String placement) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_rewardedInterstitialAd != null;

        if (m_showing == true) {
            ready = false;
        }

        this.log("showRewardedInterstitial", Map.of("placement", placement, "ready", ready));

        this.buildRewardedInterstitialAdEvent("show")
            .addParameterString("placement", placement)
            .addParameterBoolean("ready", ready)
            .log();

        if (ready == false) {
            return false;
        }

        m_showing = true;
        m_placement = placement;

        RewardedInterstitialAd show_rewardedAd = m_rewardedInterstitialAd;

        boolean scheduled = this.performOnMainThread(() -> {
            show_rewardedAd.show(activity, rewardItem -> {
                MengineAdMobRewardedInterstitialAd.this.performOnMainThread(() -> {
                    String rewardType = rewardItem.getType();
                    int rewardAmount = rewardItem.getAmount();

                    MengineAdMobRewardedInterstitialAd.this.log("onUserEarnedReward");

                    MengineAdMobRewardedInterstitialAd.this.buildRewardedInterstitialAdEvent("user_rewarded")
                        .addParameterString("placement", placement)
                        .addParameterString("reward_type", rewardType)
                        .addParameterLong("reward_amount", rewardAmount)
                        .log();

                    MengineAdResponseInterface adResponse = m_adService.getAdResponse();

                    adResponse.onAdUserRewarded(MengineAdMediation.ADMEDIATION_ADMOB, MengineAdFormat.ADFORMAT_REWARDED_INTERSTITIAL, placement, rewardType, rewardAmount);
                });
            });
        });

        if (scheduled == false) {
            m_showing = false;
            m_placement = "";

            return false;
        }

        return true;
    }

    @Override
    public boolean isShowingRewardedInterstitial() {
        return m_showing;
    }

    private static class LoadCallback extends RewardedInterstitialAdLoadCallback {
        private MengineAdMobRewardedInterstitialAd m_ad;

        public LoadCallback(@NonNull MengineAdMobRewardedInterstitialAd ad) {
            m_ad = ad;
        }

        public void destroy() {
            m_ad = null;
        }

        @Override
        public void onAdLoaded(@NonNull RewardedInterstitialAd ad) {
            if (m_ad == null) {
                return;
            }

            m_ad.handleAdLoaded(ad);
        }

        @Override
        public void onAdFailedToLoad(@NonNull LoadAdError error) {
            if (m_ad == null) {
                return;
            }

            m_ad.handleAdFailedToLoad(error);
        }
    }
}
