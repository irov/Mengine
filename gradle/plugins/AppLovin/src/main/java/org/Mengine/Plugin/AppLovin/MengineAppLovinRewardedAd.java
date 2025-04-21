package org.Mengine.Plugin.AppLovin;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

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
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAnalyticsEventBuilder;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

public class MengineAppLovinRewardedAd extends MengineAppLovinBase implements MaxAdRequestListener, MaxRewardedAdListener, MaxAdRevenueListener, MaxAdExpirationListener, MaxAdReviewListener {
    private MaxRewardedAd m_rewardedAd;

    public MengineAppLovinRewardedAd(@NonNull MengineAppLovinPlugin plugin, String adUnitId) {
        super(plugin, adUnitId, MaxAdFormat.REWARDED);

        //Empty
    }

    protected MengineAnalyticsEventBuilder buildRewardedAdEvent(@Size(min = 1L,max = 40L) String event) {
        MengineAnalyticsEventBuilder builder = this.buildAdEvent("mng_ad_rewarded_" + event)
            ;

        return builder;
    }

    protected void setRewardedState(@NonNull String state) {
        this.setState("applovin.rewarded.state." + m_adUnitId, state);
    }

    @Override
    public boolean isInitialized() {
        return m_rewardedAd != null;
    }

    @Override
    public void initialize(@NonNull MengineApplication application) {
        super.initialize(application);

        MaxRewardedAd rewardedAd = MaxRewardedAd.getInstance(m_adUnitId);

        rewardedAd.setListener(this);
        rewardedAd.setRequestListener(this);
        rewardedAd.setRevenueListener(this);
        rewardedAd.setExpirationListener(this);
        rewardedAd.setAdReviewListener(this);

        m_rewardedAd = rewardedAd;

        this.log("create");

        this.setRewardedState("init");

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            try {
                mediationAmazon.loadMediatorRewarded(application, m_plugin, m_rewardedAd, () -> {
                    this.loadAd();
                });
            } catch (final MengineServiceInvalidInitializeException e) {
                m_plugin.logError("loadMediatorRewarded exception: %s", e);

                this.loadAd();
            }
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

        this.buildRewardedAdEvent("load")
            .log();

        this.setRewardedState("load");

        try {
            m_rewardedAd.loadAd();
        } catch (final Exception e) {
            this.logError("loadAd", e);

            this.buildRewardedAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.retryLoadAd();
        }
    }

    public boolean canOfferRewarded(String placement) {
        if (m_rewardedAd == null) {
            return false;
        }

        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_rewardedAd.isReady();

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
        if (m_rewardedAd == null) {
            return false;
        }

        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_rewardedAd.isReady();

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
        if (m_rewardedAd == null) {
            return false;
        }

        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_rewardedAd.isReady();

        this.log("showRewarded", Map.of("placement", placement, "ready", ready));

        this.buildRewardedAdEvent("show")
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

        this.buildRewardedAdEvent("request_started")
            .log();

        this.setRewardedState("request_started");
    }

    @Override
    public void onUserRewarded(@NonNull MaxAd ad, @NonNull MaxReward reward) {
        this.logMaxAdReward("onUserRewarded", ad, reward);

        String placement = ad.getPlacement();

        String label = reward.getLabel();
        int amount = reward.getAmount();

        this.buildRewardedAdEvent("user_rewarded")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterString("reward_label", label)
            .addParameterLong("reward_amount", amount)
            .log();

        m_adResponse.onAdUserRewarded(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_REWARDED, placement, label, amount);
    }

    @Override
    public void onAdLoaded(@NonNull MaxAd ad) {
        this.logMaxAd("onAdLoaded", ad);

        this.buildRewardedAdEvent("loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setRewardedState("loaded." + ad.getNetworkName());

        m_requestAttempt = 0;
    }

    @Override
    public void onAdDisplayed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        String placement = ad.getPlacement();

        this.buildRewardedAdEvent("displayed")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setRewardedState("displayed." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdHidden(@NonNull MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        String placement = ad.getPlacement();

        this.buildRewardedAdEvent("hidden")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setRewardedState("hidden." + placement + "." + ad.getNetworkName());

        MengineUtils.performOnMainThread(() -> {
            m_adResponse.onAdShowSuccess(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_REWARDED, placement);

            this.loadAd();
        });
    }

    @Override
    public void onAdClicked(@NonNull MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        String placement = ad.getPlacement();

        this.buildRewardedAdEvent("clicked")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setRewardedState("clicked." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildRewardedAdEvent("load_failed")
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterLong("error_code", errorCode)
            .log();

        this.setRewardedState("load_failed." + errorCode);

        this.retryLoadAd();
    }

    @Override
    public void onAdDisplayFailed(@NonNull MaxAd ad, @NonNull MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildRewardedAdEvent("display_failed")
            .addParameterString("placement", placement)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        this.setRewardedState("display_failed." + placement + "." + ad.getNetworkName() + "." + errorCode);

        MengineUtils.performOnMainThread(() -> {
            m_adResponse.onAdShowFailed(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_REWARDED, placement, errorCode);

            this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        String placement = ad.getPlacement();

        this.buildRewardedAdEvent("revenue_paid")
            .addParameterString("placement", placement)
            .addParameterDouble("revenue", ad.getRevenue())
            .addParameterString("revenue_precision", ad.getRevenuePrecision())
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        double revenue = ad.getRevenue();

        m_adResponse.onAdRevenuePaid(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_REWARDED, placement, revenue);
    }

    @Override
    public void onExpiredAdReloaded(@NonNull MaxAd adOld, @NonNull MaxAd adNew) {
        this.logMaxAd("onExpiredAdReloaded", adOld);

        this.buildRewardedAdEvent("expired_reloaded")
            .addParameterJSON("old_ad", this.getMAAdParams(adOld))
            .addParameterJSON("new_ad", this.getMAAdParams(adNew))
            .log();
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
