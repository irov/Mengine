package org.Mengine.Plugin.AppLovin;

import androidx.annotation.NonNull;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdExpirationListener;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxReward;
import com.applovin.mediation.MaxRewardedAdListener;
import com.applovin.mediation.ads.MaxRewardedAd;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

public class MengineAppLovinRewarded extends MengineAppLovinBase implements MaxAdRequestListener, MaxRewardedAdListener, MaxAdRevenueListener, MaxAdExpirationListener {
    private MaxRewardedAd m_rewardedAd;

    public MengineAppLovinRewarded(MengineAppLovinPlugin plugin, String adUnitId) {
        super(MaxAdFormat.REWARDED, plugin, adUnitId);

        MengineActivity activity = m_plugin.getMengineActivity();

        MaxRewardedAd rewardedAd = MaxRewardedAd.getInstance(adUnitId, activity);

        rewardedAd.setListener(this);
        rewardedAd.setRequestListener(this);
        rewardedAd.setRevenueListener(this);
        rewardedAd.setExpirationListener(this);

        m_rewardedAd = rewardedAd;

        m_plugin.logMessage("[%s] create adUnitId: %s"
            , m_adFormat.getLabel()
            , adUnitId
        );

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "init");

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

        if (m_plugin.hasOption("applovin.rewarded.no_load") == true) {
            return;
        }

        this.log("loadAd");

        this.increaseRequestId();

        this.buildAdEvent("mng_ad_rewarded_load")
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "load");

        try {
            m_rewardedAd.loadAd();
        } catch (Exception e) {
            this.logError("loadAd", e);

            this.buildAdEvent("mng_ad_rewarded_load_exception")
                .addParameterException("exception", e)
                .log();

            this.retryLoadAd();
        }
    }

    public boolean canOfferRewarded(String placement) {
        if(m_rewardedAd == null) {
            return false;
        }

        boolean ready = m_rewardedAd.isReady();

        this.log("canOfferRewarded", Map.of("placement", placement, "ready", ready));

        this.buildAdEvent("mng_ad_rewarded_offer")
            .addParameterString("placement", placement)
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

        boolean ready = m_rewardedAd.isReady();

        this.log("canYouShowRewarded", Map.of("placement", placement, "ready", ready));

        if (ready == false) {
            this.buildAdEvent("mng_ad_rewarded_show")
                .addParameterString("placement", placement)
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

        boolean ready = m_rewardedAd.isReady();

        this.log("showRewarded", Map.of("placement", placement, "ready", ready));

        this.buildAdEvent("mng_ad_rewarded_show")
            .addParameterString("placement", placement)
            .addParameterBoolean("ready", ready)
            .log();

        if (ready == false) {
            return false;
        }

        m_rewardedAd.showAd(placement);

        return true;
    }

    @Override
    public void onAdRequestStarted(@NonNull String adUnitId) {
        this.log("onAdRequestStarted");

        this.buildAdEvent("mng_ad_rewarded_request_started")
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "request_started");

        m_plugin.pythonCall("onApplovinRewardedOnAdRequestStarted", m_adUnitId);
    }

    @SuppressWarnings("deprecation")
    @Override
    public void onRewardedVideoStarted(MaxAd ad) {
        this.logMaxAd("onRewardedVideoStarted", ad);

        //Deprecation
    }

    @SuppressWarnings("deprecation")
    @Override
    public void onRewardedVideoCompleted(MaxAd ad) {
        this.logMaxAd("onRewardedVideoCompleted", ad);

        //Deprecation
    }

    @Override
    public void onUserRewarded(MaxAd ad, MaxReward reward) {
        this.logMaxAdReward("onUserRewarded", ad, reward);

        String placement = ad.getPlacement();

        String label = reward.getLabel();
        int amount = reward.getAmount();

        this.buildAdEvent("mng_ad_rewarded_user_rewarded")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterString("reward_label", label)
            .addParameterLong("reward_amount", amount)
            .log();

        m_plugin.pythonCall("onApplovinRewardedOnUserRewarded", m_adUnitId, label, amount);
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        this.logMaxAd("onAdLoaded", ad);

        this.buildAdEvent("mng_ad_rewarded_loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "loaded." + ad.getNetworkName());

        m_retryAttempt = 0;

        m_plugin.pythonCall("onApplovinRewardedOnAdLoaded", m_adUnitId);
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_rewarded_displayed")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "displayed." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinRewardedOnAdDisplayed", m_adUnitId);
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_rewarded_hidden")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "hidden." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinRewardedOnAdHidden", m_adUnitId);

        MengineUtils.performOnMainThread(() -> {
            MengineAppLovinRewarded.this.loadAd();
        });
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_rewarded_clicked")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "clicked." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinRewardedOnAdClicked", m_adUnitId);
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_rewarded_load_failed")
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterLong( "error_code", errorCode)
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "load_failed");

        m_plugin.pythonCall("onApplovinRewardedOnAdLoadFailed", m_adUnitId);

        this.retryLoadAd();
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_rewarded_display_failed")
            .addParameterString("placement", placement)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "display_failed." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinRewardedOnAdDisplayFailed", m_adUnitId);

        MengineUtils.performOnMainThread(() -> {
            MengineAppLovinRewarded.this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        this.buildAdEvent("mng_ad_rewarded_revenue_paid")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        m_plugin.pythonCall("onApplovinRewardedOnAdRevenuePaid", m_adUnitId);
    }

    @Override
    public void onExpiredAdReloaded(@NonNull MaxAd adOld, @NonNull MaxAd adNew) {
        this.logMaxAd("onExpiredAdReloaded", adOld);

        this.buildAdEvent("mng_ad_rewarded_expired_reloaded")
            .addParameterJSON("old_ad", this.getMAAdParams(adOld))
            .addParameterJSON("new_ad", this.getMAAdParams(adNew))
            .log();
    }
}
