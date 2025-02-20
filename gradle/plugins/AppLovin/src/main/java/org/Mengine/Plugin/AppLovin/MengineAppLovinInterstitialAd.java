package org.Mengine.Plugin.AppLovin;

import androidx.annotation.NonNull;

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
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

public class MengineAppLovinInterstitialAd extends MengineAppLovinBase implements MaxAdListener, MaxAdRequestListener, MaxAdRevenueListener, MaxAdExpirationListener, MaxAdReviewListener {
    private MaxInterstitialAd m_interstitialAd;

    public MengineAppLovinInterstitialAd(MengineAppLovinPlugin plugin, String adUnitId) {
        super(plugin, adUnitId, MaxAdFormat.INTERSTITIAL);

        //Empty
    }

    @Override
    public void initialize(@NonNull MengineApplication application) {
        super.initialize(application);

        MaxInterstitialAd interstitialAd = new MaxInterstitialAd(m_adUnitId, application);

        interstitialAd.setListener(this);
        interstitialAd.setRequestListener(this);
        interstitialAd.setRevenueListener(this);
        interstitialAd.setExpirationListener(this);
        interstitialAd.setAdReviewListener(this);

        m_interstitialAd = interstitialAd;

        this.log("create");

        this.setState("applovin.interstitial.state." + m_adUnitId, "init");

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            mediationAmazon.loadMediatorInterstitial(application, m_interstitialAd, () -> {
                this.loadAd();
            });
        } else {
            this.loadAd();
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

        if (m_plugin.hasOption("applovin.interstitial.no_load") == true || m_plugin.hasOption("applovin.ad.no_load") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildAdEvent("mng_ad_interstitial_load")
            .log();

        this.setState("applovin.interstitial.state." + m_adUnitId, "load");

        try {
            m_interstitialAd.loadAd();
        } catch (final Exception e) {
            this.logError("loadAd", e);

            this.buildAdEvent("mng_ad_interstitial_load_exception")
                .addParameterException("exception", e)
                .log();

            this.setState("applovin.interstitial.state." + m_adUnitId, "load_exception");

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
            this.buildAdEvent("mng_ad_interstitial_show")
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

        this.buildAdEvent("mng_ad_interstitial_show")
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

        this.buildAdEvent("mng_ad_interstitial_request_started")
            .log();

        this.setState("applovin.interstitial.state." + m_adUnitId, "request_started");
    }

    @Override
    public void onAdLoaded(@NonNull MaxAd ad) {
        this.logMaxAd("onAdLoaded", ad);

        this.buildAdEvent("mng_ad_interstitial_loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setState("applovin.interstitial.state." + m_adUnitId, "loaded." + ad.getNetworkName());

        m_requestAttempt = 0;
    }

    @Override
    public void onAdDisplayed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_interstitial_displayed")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setState("applovin.interstitial.state." + m_adUnitId, "displayed." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdHidden(@NonNull MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_interstitial_hidden")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setState("applovin.interstitial.state." + m_adUnitId, "hidden." + placement + "." + ad.getNetworkName());

        MengineUtils.performOnMainThread(() -> {
            this.pythonCall("onAndroidAppLovinInterstitialShowSuccessful", Map.of("placement", placement));

            this.loadAd();
        });
    }

    @Override
    public void onAdClicked(@NonNull MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_interstitial_clicked")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setState("applovin.interstitial.state." + m_adUnitId, "clicked." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_interstitial_load_failed")
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        this.setState("applovin.interstitial.state." + m_adUnitId, "load_failed." + errorCode);

        this.retryLoadAd();
    }

    @Override
    public void onAdDisplayFailed(@NonNull MaxAd ad, @NonNull MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_interstitial_display_failed")
            .addParameterString("placement", placement)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setState("applovin.interstitial.state." + m_adUnitId, "display_failed." + placement + "." + ad.getNetworkName() + "." + errorCode);

        MengineUtils.performOnMainThread(() -> {
            this.pythonCall("onAndroidAppLovinInterstitialShowFailed", Map.of("placement", placement, "error_code", errorCode));

            this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_interstitial_revenue_paid")
            .addParameterString("placement", placement)
            .addParameterDouble("revenue", ad.getRevenue())
            .addParameterString("revenue_precision", ad.getRevenuePrecision())
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

		double revenue = ad.getRevenue();

        this.pythonCall("onAndroidAppLovinInterstitialRevenuePaid", Map.of("placement", placement, "revenue", revenue));
    }

    @Override
    public void onExpiredAdReloaded(@NonNull MaxAd adOld, @NonNull MaxAd adNew) {
        this.logMaxAd("onExpiredAdReloaded", adOld);

        this.buildAdEvent("mng_ad_interstitial_expired_reloaded")
            .addParameterJSON("old_ad", this.getMAAdParams(adOld))
            .addParameterJSON("new_ad", this.getMAAdParams(adNew))
            .log();
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
