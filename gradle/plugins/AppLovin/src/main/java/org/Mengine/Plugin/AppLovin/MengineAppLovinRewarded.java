package org.Mengine.Plugin.AppLovin;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxReward;
import com.applovin.mediation.MaxRewardedAdListener;
import com.applovin.mediation.ads.MaxRewardedAd;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineUtils;

import java.util.concurrent.TimeUnit;

public class MengineAppLovinRewarded extends MengineAppLovinBase implements MaxAdRequestListener, MaxRewardedAdListener, MaxAdRevenueListener {
    private MaxRewardedAd m_rewardedAd;

    private int m_retryAttempt;

    private int m_enumeratorRequest;
    private int m_requestId;

    public MengineAppLovinRewarded(MengineAppLovinPlugin plugin, String adUnitId) {
        super(plugin);

        m_retryAttempt = 0;
        m_enumeratorRequest = 0;
        m_requestId = 0;

        MengineActivity activity = m_plugin.getMengineActivity();

        MaxRewardedAd rewardedAd = MaxRewardedAd.getInstance(adUnitId, activity);

        rewardedAd.setRequestListener(this);
        rewardedAd.setListener(this);
        rewardedAd.setRevenueListener(this);

        m_rewardedAd = rewardedAd;

        m_plugin.logMessage("[Rewarded] create adUnitId: %s"
            , adUnitId
        );

        m_plugin.setState("applovin.rewarded.state." + m_rewardedAd.getAdUnitId(), "init");

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            mediationAmazon.loadMediatorRewarded(activity, plugin, m_rewardedAd, new MengineAppLovinMediationLoadAdCallback() {
                @Override
                public void onLoadAd() {
                    MengineAppLovinRewarded.this.loadAd();
                }
            });
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

        String adUnitId = m_rewardedAd.getAdUnitId();

        m_plugin.logInfo("[Rewarded] loadAd adUnitId: %s request: %d attempt: %d"
            , adUnitId
            , m_enumeratorRequest
            , m_retryAttempt
        );

        m_requestId = m_enumeratorRequest++;

        this.buildEvent("mng_ad_rewarded_load")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("attempt", m_retryAttempt)
            .log();

        m_plugin.setState("applovin.rewarded.state." + adUnitId, "load");

        m_rewardedAd.loadAd();
    }

    public boolean canOfferRewarded(String placement) {
        if(m_rewardedAd == null) {
            return false;
        }

        String adUnitId = m_rewardedAd.getAdUnitId();

        boolean ready = m_rewardedAd.isReady();

        m_plugin.logMessage("[Rewarded] canOfferRewarded adUnitId: %s ready: %b request: %d attempt: %d"
            , adUnitId
            , ready
            , m_requestId
            , m_retryAttempt
        );

        this.buildEvent("mng_ad_rewarded_offer")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("attempt", m_retryAttempt)
            .addParameterBoolean("ready", ready)
            .log();

        if (ready == false) {
            return false;
        }

        return true;
    }

    public boolean canYouShowRewarded(String placement) {
        if(m_rewardedAd == null) {
            return false;
        }

        String adUnitId = m_rewardedAd.getAdUnitId();

        boolean ready = m_rewardedAd.isReady();

        m_plugin.logMessage("[Rewarded] canYouShowRewarded adUnitId: %s ready: %b request: %d attempt: %d"
            , adUnitId
            , ready
            , m_requestId
            , m_retryAttempt
        );

        if (ready == false) {
            this.buildEvent("mng_ad_rewarded_show")
                .addParameterString("ad_unit_id", adUnitId)
                .addParameterString("placement", placement)
                .addParameterLong("request_id", m_requestId)
                .addParameterLong("attempt", m_retryAttempt)
                .addParameterBoolean("ready", false)
                .log();

            return false;
        }

        return true;
    }

    public boolean showRewarded(String placement) {
        if(m_rewardedAd == null) {
            return false;
        }

        String adUnitId = m_rewardedAd.getAdUnitId();

        boolean ready = m_rewardedAd.isReady();

        m_plugin.logMessage("[Rewarded] showRewarded adUnitId: %s ready: %b request: %d attempt: %d"
            , adUnitId
            , ready
            , m_requestId
            , m_retryAttempt
        );

        this.buildEvent("mng_ad_rewarded_show")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("attempt", m_retryAttempt)
            .addParameterBoolean("ready", ready)
            .log();

        if (ready == false) {
            return false;
        }

        m_rewardedAd.showAd(placement);

        return true;
    }

    @Override
    public void onAdRequestStarted(String adUnitId) {
        m_plugin.logInfo("[Rewarded] onAdRequestStarted adUnitId: %s request: %d attempt: %d"
            , adUnitId
            , m_requestId
            , m_retryAttempt
        );

        this.buildEvent("mng_ad_rewarded_request_started")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("attempt", m_retryAttempt)
            .log();

        m_plugin.setState("applovin.rewarded.state." + adUnitId, "request_started");

        m_plugin.pythonCall("onApplovinRewardedOnAdRequestStarted", adUnitId);
    }

    @SuppressWarnings("deprecation")
    @Override
    public void onRewardedVideoStarted(MaxAd ad) {
        this.logMaxAd("Rewarded", "onRewardedVideoStarted", ad);
    }

    @SuppressWarnings("deprecation")
    @Override
    public void onRewardedVideoCompleted(MaxAd ad) {
        this.logMaxAd("Rewarded", "onRewardedVideoCompleted", ad);
    }

    @Override
    public void onUserRewarded(MaxAd ad, MaxReward reward) {
        this.logMaxAd("Rewarded", "onUserRewarded", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        String label = reward.getLabel();
        int amount = reward.getAmount();

        m_plugin.logInfo("MaxReward %s [%d]"
            , label
            , amount
        );

        this.buildEvent("mng_ad_rewarded_user_rewarded")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterString("label", label)
            .addParameterLong("amount", amount)
            .log();

        m_plugin.pythonCall("onApplovinRewardedOnUserRewarded", adUnitId, label, amount);
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdLoaded", ad);

        String adUnitId = ad.getAdUnitId();

        this.buildEvent("mng_ad_rewarded_loaded")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("attempt", m_retryAttempt)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + adUnitId, "loaded." + ad.getNetworkName());

        m_retryAttempt = 0;

        m_plugin.pythonCall("onApplovinRewardedOnAdLoaded", adUnitId);
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdDisplayed", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_rewarded_displayed")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + adUnitId, "displayed." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinRewardedOnAdDisplayed", adUnitId);
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdHidden", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_rewarded_hidden")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + adUnitId, "hidden." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinRewardedOnAdHidden", adUnitId);

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdClicked", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_rewarded_clicked")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + adUnitId, "clicked." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinRewardedOnAdClicked", adUnitId);
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        this.logMaxError("Rewarded", "onAdLoadFailed", adUnitId, error);

        int errorCode = error.getCode();

        this.buildEvent("mng_ad_rewarded_load_failed")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("attempt", m_retryAttempt)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterLong( "error_code", errorCode)
            .log();

        m_plugin.setState("applovin.rewarded.state." + adUnitId, "load_failed");

        m_plugin.pythonCall("onApplovinRewardedOnAdLoadFailed", adUnitId);

        m_retryAttempt++;

        long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttempt)));

        MengineUtils.performOnMainThreadDelayed(() -> {
            this.loadAd();
        }, delayMillis);
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("Rewarded", "onAdDisplayFailed", ad.getAdUnitId(), error);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildEvent("mng_ad_rewarded_display_failed")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.rewarded.state." + adUnitId, "display_failed." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinRewardedOnAdDisplayFailed", adUnitId);

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("Rewarded", "onAdRevenuePaid", ad);

        String adUnitId = ad.getAdUnitId();

        this.buildEvent("mng_ad_rewarded_revenue_paid")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        m_plugin.pythonCall("onApplovinRewardedOnAdRevenuePaid", adUnitId);
    }
}
