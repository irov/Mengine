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
    private MaxRewardedAd m_rewardedAd;

    private int m_retryAttemptRewarded;

    private int m_enumeratorRequest;
    private int m_requestId;

    public MengineAppLovinRewarded(MengineAppLovinPlugin plugin, String adUnitId) {
        super(plugin);

        m_retryAttemptRewarded = 0;
        m_enumeratorRequest = 0;
        m_requestId = 0;

        MengineActivity activity = m_plugin.getActivity();

        MaxRewardedAd rewardedAd = MaxRewardedAd.getInstance(adUnitId, activity);

        rewardedAd.setRequestListener(this);
        rewardedAd.setListener(this);
        rewardedAd.setRevenueListener(this);

        m_rewardedAd = rewardedAd;

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            try {
                mediationAmazon.loadMediatorRewarded(activity, m_rewardedAd);
            } catch (Exception e) {
            }
        } else {
            this.loadAd();
        }
    }

    @Override
    public void destroy() {
        super.destroy();

        if (m_rewardedAd != null) {
            m_rewardedAd.destroy();
            m_rewardedAd = null;
        }
    }

    public void loadAd() {
        if (m_rewardedAd == null) {
            return;
        }

        m_plugin.logInfo("[Rewarded] loadAd");

        m_requestId = m_enumeratorRequest++;

        this.buildEvent("ad_rewarded_load")
            .addParameterInteger("request_id", m_requestId)
            .log();

        m_rewardedAd.loadAd();
    }

    public boolean canOfferRewarded() {
        boolean ready = m_rewardedAd.isReady();

        m_plugin.logMessage("[Rewarded] canOfferRewarded [%d]"
            , ready
        );

        if (ready == false) {
            this.buildEvent("ad_rewarded_offer_unready")
                .addParameterInteger("request_id", m_requestId)
                .log();

            return false;
        }

        this.buildEvent("ad_rewarded_offer")
            .addParameterInteger("request_id", m_requestId)
            .log();

        return true;
    }

    public boolean canYouShowRewarded() {
        boolean ready = m_rewardedAd.isReady();

        m_plugin.logMessage("[Rewarded] canYouShowRewarded [%d]"
            , ready
        );

        if (ready == false) {
            this.buildEvent("ad_rewarded_show_unready")
                .addParameterInteger("request_id", m_requestId)
                .log();

            return false;
        }

        return true;
    }

    public boolean showRewarded() {
        boolean ready = m_rewardedAd.isReady();

        m_plugin.logMessage("[Rewarded] showInterstitial [%d]"
            , ready
        );

        if (ready == false) {
            this.buildEvent("ad_rewarded_show_unready")
                .addParameterInteger("request_id", m_requestId)
                .log();

            return false;
        }

        this.buildEvent("ad_rewarded_show")
            .addParameterInteger("request_id", m_requestId)
            .log();

        m_rewardedAd.showAd();

        return true;
    }

    @Override
    public void onAdRequestStarted(String adUnitId) {
        m_plugin.logInfo("[Rewarded] onAdRequestStarted %s"
            , adUnitId
        );

        this.buildEvent("ad_rewarded_request_started")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("unit_ad", adUnitId)
            .log();

        m_plugin.pythonCall("onApplovinRewardedOnAdRequestStarted");
    }

    @Override @Deprecated
    public void onRewardedVideoStarted(MaxAd ad) {
        this.logMaxAd("Rewarded", "onRewardedVideoStarted", ad);

        this.buildEvent("ad_rewarded_video_started")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinRewardedOnRewardedVideoStarted");
    }

    @Override @Deprecated
    public void onRewardedVideoCompleted(MaxAd ad) {
        this.logMaxAd("Rewarded", "onRewardedVideoCompleted", ad);

        this.buildEvent("ad_rewarded_video_completed")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

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

        this.buildEvent("ad_rewarded_user_rewarded")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .addParameterString("label", label)
            .addParameterInteger("amount", amount)
            .log();

        m_plugin.pythonCall("onApplovinRewardedOnUserRewarded", label, amount);
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdLoaded", ad);

        this.buildEvent("ad_rewarded_loaded")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_retryAttemptRewarded = 0;

        m_plugin.pythonCall("onApplovinRewardedOnAdLoaded");
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdDisplayed", ad);

        this.buildEvent("ad_rewarded_displayed")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinRewardedOnAdDisplayed");
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdHidden", ad);

        this.buildEvent("ad_rewarded_hidden")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });

        m_plugin.pythonCall("onApplovinRewardedOnAdHidden");
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdClicked", ad);

        this.buildEvent("ad_rewarded_clicked")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinRewardedOnAdClicked");
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        this.logMaxError("Rewarded", "onAdLoadFailed", error);

        this.buildEvent("ad_rewarded_load_failed")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("unit_id", adUnitId)
            .addParameterString("error", this.getMaxErrorParams(error))
            .addParameterInteger( "error_code", error.getCode())
            .log();

        m_retryAttemptRewarded++;

        long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptRewarded)));

        MengineUtils.performOnMainThreadWithDelay(() -> {
            this.loadAd();
        }, delayMillis);

        m_plugin.pythonCall("onApplovinRewardedOnAdLoadFailed");
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("Rewarded", "onAdDisplayFailed", error);

        this.buildEvent("ad_rewarded_display_failed")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .addParameterString("error", this.getMaxErrorParams(error))
            .addParameterInteger( "error_code", error.getCode())
            .log();

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });

        m_plugin.pythonCall("onApplovinRewardedOnAdDisplayFailed");
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdRevenuePaid", ad);

        this.buildEvent("ad_rewarded_revenue_paid")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        m_plugin.pythonCall("onApplovinRewardedOnAdRevenuePaid");
    }
}
