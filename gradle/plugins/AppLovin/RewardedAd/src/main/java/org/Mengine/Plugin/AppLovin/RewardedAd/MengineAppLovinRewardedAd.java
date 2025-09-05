package org.Mengine.Plugin.AppLovin.RewardedAd;

import androidx.annotation.NonNull;
import androidx.annotation.Size;
import androidx.annotation.StringRes;

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
import org.Mengine.Base.MengineAdResponseInterface;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineAnalyticsEventBuilderInterface;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinBase;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinPluginInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinRewardedAdInterface;

import java.util.Map;

public class MengineAppLovinRewardedAd extends MengineAppLovinBase implements MengineAppLovinRewardedAdInterface, MaxAdRequestListener, MaxRewardedAdListener, MaxAdRevenueListener, MaxAdExpirationListener, MaxAdReviewListener {
    public static final @StringRes int METADATA_REWARDED_ADUNITID = R.string.mengine_applovin_rewarded_adunitid;

    private MaxRewardedAd m_rewardedAd;

    public MengineAppLovinRewardedAd(@NonNull MengineAdService adService, @NonNull MengineAppLovinPluginInterface plugin) throws MengineServiceInvalidInitializeException {
        super(adService, plugin, MaxAdFormat.REWARDED);

        String MengineAppLovinPlugin_Rewarded_AdUnitId = plugin.getResourceString(METADATA_REWARDED_ADUNITID);

        if (MengineAppLovinPlugin_Rewarded_AdUnitId.isEmpty() == true) {
            this.invalidInitialize("meta %s is empty"
                , METADATA_REWARDED_ADUNITID
            );
        }

        this.setAdUnitId(MengineAppLovinPlugin_Rewarded_AdUnitId);
    }

    protected MengineAnalyticsEventBuilderInterface buildRewardedAdEvent(@Size(min = 1L,max = 40L) String event) {
        MengineAnalyticsEventBuilderInterface builder = this.buildAdEvent("mng_applovin_rewarded_" + event);

        return builder;
    }

    protected void setRewardedState(@NonNull String state) {
        this.setState("applovin.rewarded.state." + m_adUnitId, state);
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) {
        super.onActivityCreate(activity);

        MaxRewardedAd rewardedAd = MaxRewardedAd.getInstance(m_adUnitId);

        rewardedAd.setListener(this);
        rewardedAd.setRequestListener(this);
        rewardedAd.setRevenueListener(this);
        rewardedAd.setExpirationListener(this);
        rewardedAd.setAdReviewListener(this);

        m_rewardedAd = rewardedAd;

        this.log("create");

        this.setRewardedState("init");

        this.firstLoadAd((mediation, callback) -> {
            mediation.loadMediatorRewarded(activity, m_plugin, m_rewardedAd, callback);
        });
    }

    @Override
    public void onActivityDestroy(@NonNull MengineActivity activity) {
        super.onActivityDestroy(activity);

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

        if (m_plugin.hasOption("applovin.rewarded.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
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

        MengineAdResponseInterface adResponse = m_adService.getAdResponse();

        adResponse.onAdUserRewarded(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_REWARDED, placement, label, amount);
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

        m_adService.setRewardedAdShowing(true);
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

        m_adService.setRewardedAdShowing(false);

        MengineUtils.performOnMainThread(() -> {
            MengineAdResponseInterface adResponse = m_adService.getAdResponse();

            adResponse.onAdShowSuccess(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_REWARDED, placement);

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
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterLong("error_code", errorCode)
            .log();

        this.setRewardedState("display_failed." + placement + "." + ad.getNetworkName() + "." + errorCode);

        m_adService.setRewardedAdShowing(false);

        MengineUtils.performOnMainThread(() -> {
            MengineAdResponseInterface adResponse = m_adService.getAdResponse();

            adResponse.onAdShowFailed(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_REWARDED, placement, errorCode);

            this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        this.revenuePaid(ad);
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
