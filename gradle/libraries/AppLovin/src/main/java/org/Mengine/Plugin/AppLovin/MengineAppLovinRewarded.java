package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineUtils;
import org.Mengine.Base.MengineActivity;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxReward;
import com.applovin.mediation.MaxRewardedAdListener;
import com.applovin.mediation.ads.MaxRewardedAd;

import java.util.concurrent.TimeUnit;

public class MengineAppLovinRewarded extends MengineAppLovinBase implements MaxAdRequestListener, MaxRewardedAdListener, MaxAdRevenueListener {
    private MengineAppLovinPlugin m_plugin;

    private MaxRewardedAd m_rewardedAd;

    private int m_retryAttemptRewarded;

    public MengineAppLovinRewarded(MengineAppLovinPlugin plugin) throws Exception {
        super(plugin);

        m_retryAttemptRewarded = 0;

        MengineActivity activity = m_plugin.getActivity();

        String AppLovin_RewardedAdUnitId = activity.getConfigValue("AppLovinPlugin", "RewardedAdUnitId", "");

        if (AppLovin_RewardedAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] RewardedAdUnitId");
        }

        MaxRewardedAd rewardedAd = MaxRewardedAd.getInstance(AppLovin_RewardedAdUnitId, activity);

        rewardedAd.setRequestListener(this);
        rewardedAd.setListener(this);
        rewardedAd.setRevenueListener(this);

        m_rewardedAd = rewardedAd;
    }

    public void destroy() {
        super.destroy();

        if (m_rewardedAd != null) {
            m_rewardedAd.destroy();
            m_rewardedAd = null;
        }
    }

    public void loadRewarded() {
        MengineActivity activity = m_plugin.getActivity();

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            try {
                mediationAmazon.loadMediatorRewarded(activity, m_rewardedAd);
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

    @Override
    public void onAdRequestStarted(String adUnitId) {
        m_plugin.logInfo("[Rewarded] onAdRequestStarted %s"
                , adUnitId
        );

        m_plugin.pythonCall("onApplovinRewardedOnAdRequestStarted");
    }

    @Override @Deprecated
    public void onRewardedVideoStarted(MaxAd ad) {
        this.logMaxAd("Rewarded", "onRewardedVideoStarted", ad);

        m_plugin.pythonCall("onApplovinRewardedOnRewardedVideoStarted");
    }

    @Override @Deprecated
    public void onRewardedVideoCompleted(MaxAd ad) {
        this.logMaxAd("Rewarded", "onRewardedVideoCompleted", ad);

        m_plugin.pythonCall("onApplovinRewardedOnRewardedVideoCompleted");
    }

    @Override
    public void onUserRewarded(MaxAd ad, MaxReward reward) {
        this.logMaxAd("Rewarded", "onUserRewarded", ad);

        String label = reward.getLabel();
        int amount = reward.getAmount();

        m_plugin.logInfo("rewarded %s [%d]"
            , label
            , amount
        );

        m_plugin.pythonCall("onApplovinRewardedOnUserRewarded", label, amount);
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdLoaded", ad);

        m_retryAttemptRewarded = 0;

        m_plugin.pythonCall("onApplovinRewardedOnAdLoaded");
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdDisplayed", ad);

        m_plugin.pythonCall("onApplovinRewardedOnAdDisplayed");
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdHidden", ad);

        m_rewardedAd.loadAd();

        m_plugin.pythonCall("onApplovinRewardedOnAdHidden");
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdClicked", ad);

        m_plugin.pythonCall("onApplovinRewardedOnAdClicked");
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        this.logMaxError("Rewarded", "onAdLoadFailed", error);

        m_retryAttemptRewarded++;

        long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptRewarded)));

        MengineUtils.performOnMainThread(() -> {
            m_rewardedAd.loadAd();
        }, delayMillis);

        m_plugin.pythonCall("onApplovinRewardedOnAdLoadFailed");
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("Rewarded", "onAdDisplayFailed", error);

        m_rewardedAd.loadAd();

        m_plugin.pythonCall("onApplovinRewardedOnAdDisplayFailed");
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdRevenuePaid", ad);

        m_plugin.pythonCall("onApplovinRewardedOnAdRevenuePaid");

        m_plugin.eventRevenuePaid(ad);
    }
}
