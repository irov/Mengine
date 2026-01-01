package org.Mengine.Plugin.AdMob.RewardedAd;

import androidx.annotation.NonNull;
import androidx.annotation.Size;
import androidx.annotation.StringRes;

import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.OnPaidEventListener;
import com.google.android.gms.ads.OnUserEarnedRewardListener;
import com.google.android.gms.ads.ResponseInfo;
import com.google.android.gms.ads.rewarded.RewardItem;
import com.google.android.gms.ads.rewarded.RewardedAd;
import com.google.android.gms.ads.rewarded.RewardedAdLoadCallback;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdResponseInterface;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineAnalyticsEventBuilderInterface;
import org.Mengine.Base.MengineNative;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MenginePlatformEventQueue;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineTag;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobBase;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobPluginInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobRewardedAdInterface;

import java.util.Map;

public class MengineAdMobRewardedAd extends MengineAdMobBase implements MengineAdMobRewardedAdInterface {
    public static final MengineTag TAG = MengineTag.of("MNGAdMobRewardedAd");

    public static final @StringRes int METADATA_REWARDED_ADUNITID = R.string.mengine_admob_rewarded_adunitid;

    private RewardedAd m_rewardedAd;

    private boolean m_showing = false;

    public MengineAdMobRewardedAd(@NonNull MengineAdService adService, @NonNull MengineAdMobPluginInterface plugin) throws MengineServiceInvalidInitializeException {
        super(adService, plugin, MengineAdFormat.ADFORMAT_REWARDED);

        this.setAdUnitId(METADATA_REWARDED_ADUNITID, "RewardedAdUnitId");
    }

    protected MengineAnalyticsEventBuilderInterface buildRewardedAdEvent(@Size(min = 1L, max = 40L) String event) {
        MengineAnalyticsEventBuilderInterface builder = this.buildAdEvent("mng_admob_rewarded_" + event);

        return builder;
    }

    protected void setRewardedState(@NonNull String state) {
        this.setState("admob.rewarded.state." + m_adUnitId, state);
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) {
        super.onActivityCreate(activity);

        this.log("create");

        this.setRewardedState("init");

        this.loadAd();
    }

    @Override
    public void onActivityDestroy(@NonNull MengineActivity activity) {
        super.onActivityDestroy(activity);

        this.destroyRewardedAd();
    }

    private void destroyRewardedAd() {
        if (m_rewardedAd != null) {
            m_rewardedAd.setFullScreenContentCallback(null);
            m_rewardedAd.setOnPaidEventListener(null);

            m_rewardedAd = null;
        }
    }

    @Override
    public void loadAd() {
        if (m_plugin.hasOption("admob.rewarded.disable") == true || m_plugin.hasOption("admob.ad.disable") == true) {
            return;
        }

        this.log("loadAd");

        this.increaseRequestId();

        this.buildRewardedAdEvent("load")
            .log();

        this.setRewardedState("load");

        MengineActivity activity = m_plugin.getMengineActivity();

        if (activity == null) {
            return;
        }

        try {
            MengineAdResponseInterface adResponse = m_adService.getAdResponse();

            AdRequest adRequest = new AdRequest.Builder().build();

            RewardedAd.load(activity, m_adUnitId, adRequest, new RewardedAdLoadCallback() {
                @Override
                public void onAdLoaded(@NonNull RewardedAd rewardedAd) {
                    MengineAdMobRewardedAd.this.destroyRewardedAd();

                    m_rewardedAd = rewardedAd;

                    m_rewardedAd.setFullScreenContentCallback(new FullScreenContentCallback() {
                        @Override
                        public void onAdDismissedFullScreenContent() {
                            m_rewardedAd = null;

                            MengineAdMobRewardedAd.this.log("onAdDismissedFullScreenContent");

                            MengineAdMobRewardedAd.this.buildRewardedAdEvent("dismissed")
                                .log();

                            MengineAdMobRewardedAd.this.setRewardedState("dismissed");

                            MenginePlatformEventQueue.pushFreezeEvent(TAG.toString(), false);

                            m_showing = false;

                            adResponse.onAdShowSuccess(MengineAdMediation.ADMEDIATION_ADMOB, MengineAdFormat.ADFORMAT_REWARDED, null);

                            MengineUtils.performOnMainThread(() -> {
                                MengineAdMobRewardedAd.this.loadAd();
                            });
                        }

                        @Override
                        public void onAdFailedToShowFullScreenContent(@NonNull AdError adError) {
                            m_rewardedAd = null;

                            MengineAdMobRewardedAd.this.logAdError("onAdFailedToShowFullScreenContent", adError);

                            int errorCode = adError.getCode();

                            MengineAdMobRewardedAd.this.buildRewardedAdEvent("show_failed")
                                .addParameterJSON("error", MengineAdMobRewardedAd.this.getAdErrorParams(adError))
                                .addParameterLong("error_code", errorCode)
                                .log();

                            MengineAdMobRewardedAd.this.setRewardedState("show_failed." + errorCode);

                            m_showing = false;

                            adResponse.onAdShowFailed(MengineAdMediation.ADMEDIATION_ADMOB, MengineAdFormat.ADFORMAT_REWARDED, null, errorCode);

                            MengineUtils.performOnMainThread(() -> {
                                MengineAdMobRewardedAd.this.loadAd();
                            });
                        }

                        @Override
                        public void onAdShowedFullScreenContent() {
                            MengineAdMobRewardedAd.this.log("onAdShowedFullScreenContent");

                            MengineAdMobRewardedAd.this.buildRewardedAdEvent("showed")
                                .log();

                            MengineAdMobRewardedAd.this.setRewardedState("showed");

                            MenginePlatformEventQueue.pushFreezeEvent(TAG.toString(), true);
                        }

                        @Override
                        public void onAdClicked() {
                            MengineAdMobRewardedAd.this.log("onAdClicked");

                            MengineAdMobRewardedAd.this.buildRewardedAdEvent("clicked")
                                .log();

                            MengineAdMobRewardedAd.this.setRewardedState("clicked");
                        }

                        @Override
                        public void onAdImpression() {
                            MengineAdMobRewardedAd.this.log("onAdImpression");

                            MengineAdMobRewardedAd.this.buildRewardedAdEvent("impression")
                                .log();

                            MengineAdMobRewardedAd.this.setRewardedState("impression");
                        }
                    });

                    m_rewardedAd.setOnPaidEventListener(adValue -> {
                        MengineAdMobRewardedAd.this.log("onPaidEvent");

                        if (m_rewardedAd != null) {
                            ResponseInfo responseInfo = m_rewardedAd.getResponseInfo();

                            if (responseInfo != null) {
                                long valueMicros = adValue.getValueMicros();
                                double value = valueMicros / 1000000.0;

                                MengineAdMobRewardedAd.this.revenuePaid(responseInfo, MengineAdFormat.ADFORMAT_REWARDED, null, value);
                            }
                        }
                    });

                    MengineAdMobRewardedAd.this.log("onAdLoaded");

                    MengineAdMobRewardedAd.this.buildRewardedAdEvent("loaded")
                        .log();

                    MengineAdMobRewardedAd.this.setRewardedState("loaded");

                    m_requestAttempt = 0;
                }

                @Override
                public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                    MengineAdMobRewardedAd.this.destroyRewardedAd();

                    MengineAdMobRewardedAd.this.logLoadAdError("onAdFailedToLoad", loadAdError);

                    int errorCode = loadAdError.getCode();

                    MengineAdMobRewardedAd.this.buildRewardedAdEvent("load_failed")
                        .addParameterJSON("error", MengineAdMobRewardedAd.this.getLoadAdErrorParams(loadAdError))
                        .addParameterLong("error_code", errorCode)
                        .log();

                    MengineAdMobRewardedAd.this.setRewardedState("load_failed." + errorCode);

                    MengineAdMobRewardedAd.this.retryLoadAd();
                }
            });
        } catch (final Exception e) {
            this.logError("loadAd", e);

            this.buildRewardedAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.setRewardedState("load_exception");

            this.retryLoadAd();
        }
    }

    public boolean canOfferRewarded(String placement) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_rewardedAd != null;

        this.log("canOfferRewarded", Map.of("placement", placement, "ready", ready));

        this.buildRewardedAdEvent("offer")
            .addParameterString("placement", placement)
            .addParameterBoolean("ready", ready)
            .log();

        if (ready == false) {
            return false;
        }

        return true;
    }

    public boolean canYouShowRewarded(String placement) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_rewardedAd != null;

        this.log("canYouShowRewarded", Map.of("placement", placement, "ready", ready));

        if (ready == false) {
            this.buildRewardedAdEvent("show")
                .addParameterString("placement", placement)
                .addParameterBoolean("ready", false)
                .log();

            return false;
        }

        return true;
    }

    public boolean showRewarded(@NonNull MengineActivity activity, String placement) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_rewardedAd != null;

        this.log("showRewarded", Map.of("placement", placement, "ready", ready));

        this.buildRewardedAdEvent("show")
            .addParameterString("placement", placement)
            .addParameterBoolean("ready", ready)
            .log();

        if (ready == false) {
            return false;
        }

        m_showing = true;

        RewardedAd show_rewardedAd = m_rewardedAd;

        MengineUtils.performOnMainThread(() -> {
            show_rewardedAd.show(activity, new OnUserEarnedRewardListener() {
                @Override
                public void onUserEarnedReward(@NonNull RewardItem rewardItem) {
                    String rewardType = rewardItem.getType();
                    int rewardAmount = rewardItem.getAmount();

                    MengineAdMobRewardedAd.this.log("onUserEarnedReward");

                    MengineAdMobRewardedAd.this.buildRewardedAdEvent("user_rewarded")
                        .addParameterString("reward_type", rewardType)
                        .addParameterLong("reward_amount", rewardAmount)
                        .log();

                    MengineAdResponseInterface adResponse = m_adService.getAdResponse();

                    adResponse.onAdUserRewarded(MengineAdMediation.ADMEDIATION_ADMOB, MengineAdFormat.ADFORMAT_REWARDED, placement, rewardType, rewardAmount);
                }
            });
        });

        return true;
    }

    @Override
    public boolean isShowingRewarded() {
        return m_showing;
    }
}

