package org.Mengine.Plugin.AppLovin;

import androidx.annotation.NonNull;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdExpirationListener;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxAdReviewListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxReward;
import com.applovin.mediation.MaxRewardedAdListener;
import com.applovin.mediation.ads.MaxRewardedAd;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

public class MengineAppLovinRewardedAd extends MengineAppLovinBase implements MaxAdRequestListener, MaxRewardedAdListener, MaxAdRevenueListener, MaxAdExpirationListener, MaxAdReviewListener {
    private MaxRewardedAd m_rewardedAd;

    public MengineAppLovinRewardedAd(@NonNull MengineAppLovinPlugin plugin, String adUnitId) {
        super(plugin, adUnitId, MaxAdFormat.REWARDED);

        //Empty
    }

    @Override
    public boolean isInitialize() {
        return m_rewardedAd != null;
    }

    @Override
    public void initialize(@NonNull MengineApplication application) {
        super.initialize(application);

        MaxRewardedAd rewardedAd = MaxRewardedAd.getInstance(m_adUnitId, application);

        rewardedAd.setListener(this);
        rewardedAd.setRequestListener(this);
        rewardedAd.setRevenueListener(this);
        rewardedAd.setExpirationListener(this);
        rewardedAd.setAdReviewListener(this);

        m_rewardedAd = rewardedAd;

        this.log("create");

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "init");

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            mediationAmazon.loadMediatorRewarded(application, m_rewardedAd, () -> {
                this.loadAd();
            });
        } else {
            this.loadAd();
        }
    }

    @Override
    public void finalize(@NonNull MengineApplication application) {
        super.finalize(application);

        if (m_rewardedAd != null) {
            m_rewardedAd.setListener(this);
            m_rewardedAd.setRequestListener(this);
            m_rewardedAd.setRevenueListener(this);
            m_rewardedAd.setExpirationListener(this);
            m_rewardedAd.setAdReviewListener(this);

            m_rewardedAd.destroy();
            m_rewardedAd = null;
        }
    }

    @Override
    public void loadAd() {
        if (m_rewardedAd == null) {
            return;
        }

        if (m_plugin.hasOption("applovin.rewarded.no_load") == true || m_plugin.hasOption("applovin.ad.no_load") == true) {
            return;
        }

        this.log("loadAd");

        this.increaseRequestId();

        this.buildAdEvent("mng_ad_rewarded_load")
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "load");

        try {
            m_rewardedAd.loadAd();
        } catch (final Exception e) {
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

        if (MengineNetwork.isNetworkAvailable() == false) {
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

        if (MengineNetwork.isNetworkAvailable() == false) {
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

    public boolean showRewarded(MengineActivity activity, String placement) {
        if(m_rewardedAd == null) {
            return false;
        }

        if (MengineNetwork.isNetworkAvailable() == false) {
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

        m_rewardedAd.showAd(placement, activity);

        return true;
    }

    @Override
    public void onAdRequestStarted(@NonNull String adUnitId) {
        this.log("onAdRequestStarted");

        this.buildAdEvent("mng_ad_rewarded_request_started")
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "request_started");
    }

    @Override
    public void onUserRewarded(@NonNull MaxAd ad, @NonNull MaxReward reward) {
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

        m_plugin.pythonCall("onAppLovinRewardedUserRewarded", Map.of("placement", placement, "label", label, "amount", amount));
    }

    @Override
    public void onAdLoaded(@NonNull MaxAd ad) {
        this.logMaxAd("onAdLoaded", ad);

        this.buildAdEvent("mng_ad_rewarded_loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "loaded." + ad.getNetworkName());

        m_requestAttempt = 0;
    }

    @Override
    public void onAdDisplayed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_rewarded_displayed")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "displayed." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdHidden(@NonNull MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_rewarded_hidden")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "hidden." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinRewardedShowCompleted", true, Map.of("placement", placement));

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
    }

    @Override
    public void onAdClicked(@NonNull MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_rewarded_clicked")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "clicked." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_rewarded_load_failed")
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterLong( "error_code", errorCode)
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "load_failed." + errorCode);

        this.retryLoadAd();
    }

    @Override
    public void onAdDisplayFailed(@NonNull MaxAd ad, @NonNull MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_rewarded_display_failed")
            .addParameterString("placement", placement)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.rewarded.state." + m_adUnitId, "display_failed." + placement + "." + ad.getNetworkName() + "." + errorCode);

        m_plugin.pythonCall("onAppLovinRewardedShowCompleted", false, Map.of("placement", placement, "error_code", errorCode));

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_rewarded_revenue_paid")
            .addParameterString("placement", placement)
            .addParameterDouble("revenue", ad.getRevenue())
            .addParameterString("revenue_precision", ad.getRevenuePrecision())
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        double revenue = ad.getRevenue();

        m_plugin.pythonCall("onAppLovinRewardedRevenuePaid", Map.of("placement", placement, "revenue", revenue));
    }

    @Override
    public void onExpiredAdReloaded(@NonNull MaxAd adOld, @NonNull MaxAd adNew) {
        this.logMaxAd("onExpiredAdReloaded", adOld);

        this.buildAdEvent("mng_ad_rewarded_expired_reloaded")
            .addParameterJSON("old_ad", this.getMAAdParams(adOld))
            .addParameterJSON("new_ad", this.getMAAdParams(adNew))
            .log();
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
