package org.Mengine.Plugin.AppLovin;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdExpirationListener;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxAdReviewListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.ads.MaxInterstitialAd;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAnalyticsEventBuilder;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

class MengineAppLovinInterstitialAd extends MengineAppLovinBase implements MaxAdListener, MaxAdRequestListener, MaxAdRevenueListener, MaxAdExpirationListener, MaxAdReviewListener {
    private MaxInterstitialAd m_interstitialAd;

    public MengineAppLovinInterstitialAd(MengineAppLovinPlugin plugin, String adUnitId) {
        super(plugin, adUnitId, MaxAdFormat.INTERSTITIAL);

        //Empty
    }

    protected MengineAnalyticsEventBuilder buildInterstitialAdEvent(@Size(min = 1L,max = 40L) String event) {
        MengineAnalyticsEventBuilder builder = this.buildAdEvent("mng_ad_interstitial_" + event)
            ;

        return builder;
    }

    protected void setInterstitialState(@NonNull String state) {
        this.setState("applovin.interstitial.state." + m_adUnitId, state);
    }

    @Override
    public void initialize(@NonNull MengineApplication application) {
        super.initialize(application);

        MaxInterstitialAd interstitialAd = new MaxInterstitialAd(m_adUnitId);

        interstitialAd.setListener(this);
        interstitialAd.setRequestListener(this);
        interstitialAd.setRevenueListener(this);
        interstitialAd.setExpirationListener(this);
        interstitialAd.setAdReviewListener(this);

        m_interstitialAd = interstitialAd;

        this.log("create");

        this.setInterstitialState("init");

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            try {
                mediationAmazon.loadMediatorInterstitial(application, m_plugin, m_interstitialAd, () -> {
                    MengineUtils.performOnMainThread(() -> {
                        this.loadAd();
                    });
                });
            } catch (final MengineServiceInvalidInitializeException e) {
                m_plugin.logError("loadMediatorInterstitial exception: %s"
                    , e.getMessage()
                );

                MengineUtils.performOnMainThread(() -> {
                    this.loadAd();
                });
            }
        } else {
            MengineUtils.performOnMainThread(() -> {
                this.loadAd();
            });
        }
    }

    @Override
    public void finalize(@NonNull MengineApplication application) {
        super.finalize(application);

        if (m_interstitialAd != null) {
            m_interstitialAd.setListener(null);
            m_interstitialAd.setRequestListener(null);
            m_interstitialAd.setRevenueListener(null);
            m_interstitialAd.setExpirationListener(null);
            m_interstitialAd.setAdReviewListener(null);

            m_interstitialAd.destroy();
            m_interstitialAd = null;
        }
    }

    @Override
    public void loadAd() {
        if (m_interstitialAd == null) {
            return;
        }

        if (m_plugin.hasOption("applovin.interstitial.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildInterstitialAdEvent("load")
            .log();

        this.setInterstitialState("load");

        try {
            m_interstitialAd.loadAd();
        } catch (final Exception e) {
            this.logError("loadAd", e);

            this.buildInterstitialAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.setInterstitialState("load_exception");

            this.retryLoadAd();
        }
    }

    public boolean canYouShowInterstitial(String placement) {
        if (m_interstitialAd == null) {
            return false;
        }

        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_interstitialAd.isReady();

        this.log("canYouShowInterstitial", Map.of("placement", placement, "ready", ready));

        if (ready == false) {
            this.buildInterstitialAdEvent("show")
                .addParameterString("placement", placement)
                .addParameterBoolean("ready", false)
                .log();

            return false;
        }

        return true;
    }

    public boolean showInterstitial(@NonNull MengineActivity activity, String placement) {
        if (m_interstitialAd == null) {
            return false;
        }

        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_interstitialAd.isReady();

        this.log("showInterstitial", Map.of("placement", placement, "ready", ready));

        this.buildInterstitialAdEvent("show")
            .addParameterString("placement", placement)
            .addParameterBoolean("ready", ready)
            .log();

        if (ready == false) {
            return false;
        }

        m_interstitialAd.showAd(placement, activity);

        return true;
    }

    @Override
    public void onAdRequestStarted(@NonNull String adUnitId) {
        this.log("onAdRequestStarted");

        this.buildInterstitialAdEvent("request_started")
            .log();

        this.setInterstitialState("request_started");
    }

    @Override
    public void onAdLoaded(@NonNull MaxAd ad) {
        this.logMaxAd("onAdLoaded", ad);

        this.buildInterstitialAdEvent("loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setInterstitialState("loaded." + ad.getNetworkName());

        m_requestAttempt = 0;
    }

    @Override
    public void onAdDisplayed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        String placement = ad.getPlacement();

        this.buildInterstitialAdEvent("displayed")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setInterstitialState("displayed." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdHidden(@NonNull MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        String placement = ad.getPlacement();

        this.buildInterstitialAdEvent("hidden")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setInterstitialState("hidden." + placement + "." + ad.getNetworkName());

        MengineUtils.performOnMainThread(() -> {
            m_adResponse.onAdShowSuccess(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_INTERSTITIAL, placement);

            this.loadAd();
        });
    }

    @Override
    public void onAdClicked(@NonNull MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        String placement = ad.getPlacement();

        this.buildInterstitialAdEvent("clicked")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setInterstitialState("clicked." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildInterstitialAdEvent("load_failed")
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        this.setInterstitialState("load_failed." + errorCode);

        this.retryLoadAd();
    }

    @Override
    public void onAdDisplayFailed(@NonNull MaxAd ad, @NonNull MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildInterstitialAdEvent("display_failed")
            .addParameterString("placement", placement)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setInterstitialState("display_failed." + placement + "." + ad.getNetworkName() + "." + errorCode);

        MengineUtils.performOnMainThread(() -> {
            m_adResponse.onAdShowFailed(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_INTERSTITIAL, placement, errorCode);

            this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        String placement = ad.getPlacement();

        this.buildInterstitialAdEvent("revenue_paid")
            .addParameterString("placement", placement)
            .addParameterDouble("revenue", ad.getRevenue())
            .addParameterString("revenue_precision", ad.getRevenuePrecision())
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

		double revenue = ad.getRevenue();

        m_adResponse.onAdRevenuePaid(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_INTERSTITIAL, placement, revenue);
    }

    @Override
    public void onExpiredAdReloaded(@NonNull MaxAd adOld, @NonNull MaxAd adNew) {
        this.logMaxAd("onExpiredAdReloaded", adOld);

        this.buildInterstitialAdEvent("expired_reloaded")
            .addParameterJSON("old_ad", this.getMAAdParams(adOld))
            .addParameterJSON("new_ad", this.getMAAdParams(adNew))
            .log();
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
