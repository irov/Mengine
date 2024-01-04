package org.Mengine.Plugin.AppLovin;

import androidx.annotation.NonNull;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdExpirationListener;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.ads.MaxInterstitialAd;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

public class MengineAppLovinInterstitial extends MengineAppLovinBase implements MaxAdListener, MaxAdRequestListener, MaxAdRevenueListener, MaxAdExpirationListener {
    private MaxInterstitialAd m_interstitialAd;

    public MengineAppLovinInterstitial(MengineAppLovinPlugin plugin, String adUnitId) {
        super(MaxAdFormat.INTERSTITIAL, plugin, adUnitId);

        MengineActivity activity = m_plugin.getMengineActivity();

        MaxInterstitialAd interstitialAd = new MaxInterstitialAd(adUnitId, activity);

        interstitialAd.setListener(this);
        interstitialAd.setRequestListener(this);
        interstitialAd.setRevenueListener(this);
        interstitialAd.setExpirationListener(this);

        m_interstitialAd = interstitialAd;

        m_plugin.logMessage("[%s] create adUnitId: %s"
            , m_adFormat.getLabel()
            , adUnitId
        );

        m_plugin.setState("applovin.interstitial.state." + m_adUnitId, "init");

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            mediationAmazon.loadMediatorInterstitial(activity, plugin, m_interstitialAd, new MengineAppLovinMediationLoadAdCallback() {
                @Override
                public void onLoadAd() {
                    MengineAppLovinInterstitial.this.loadAd();
                }
            });
        } else {
            this.loadAd();
        }
    }

    @Override
    public void destroy() {
        super.destroy();

        if (m_interstitialAd != null) {
            m_interstitialAd.destroy();
            m_interstitialAd = null;
        }
    }

    public void loadAd() {
        if (m_interstitialAd == null) {
            return;
        }

        if (m_plugin.hasOption("applovin.interstitial.no_load") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildAdEvent("mng_ad_interstitial_load")
            .log();

        m_plugin.setState("applovin.interstitial.state." + m_adUnitId, "load");

        try {
            m_interstitialAd.loadAd();
        } catch (Exception e) {
            this.logError("loadAd", e);

            this.buildAdEvent("mng_ad_interstitial_load_exception")
                .addParameterException("exception", e)
                .log();

            m_plugin.setState("applovin.interstitial.state." + m_adUnitId, "load_exception");

            this.retryLoadAd();
        }
    }

    public boolean canYouShowInterstitial(String placement) {
        if (m_interstitialAd == null) {
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

    public boolean showInterstitial(String placement) {
        if (m_interstitialAd == null) {
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

        m_interstitialAd.showAd(placement);

        return true;
    }

    @Override
    public void onAdRequestStarted(@NonNull String adUnitId) {
        this.log("onAdRequestStarted");

        this.buildAdEvent("mng_ad_interstitial_request_started")
            .log();

        m_plugin.setState("applovin.interstitial.state." + m_adUnitId, "request_started");

        m_plugin.pythonCall("onApplovinBannerOnAdRequestStarted", m_adUnitId);
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        this.logMaxAd("onAdLoaded", ad);

        this.buildAdEvent("mng_ad_interstitial_loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.interstitial.state." + m_adUnitId, "loaded." + ad.getNetworkName());

        m_retryAttempt = 0;

        m_plugin.pythonCall("onApplovinInterstitialOnAdLoaded", m_adUnitId);
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_interstitial_displayed")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.interstitial.state." + m_adUnitId, "displayed." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinInterstitialOnAdDisplayed", m_adUnitId);
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_interstitial_hidden")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.interstitial.state." + m_adUnitId, "hidden." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinInterstitialOnAdHidden", m_adUnitId);

        MengineUtils.performOnMainThread(() -> {
            MengineAppLovinInterstitial.this.loadAd();
        });
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_interstitial_clicked")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.interstitial.state." + m_adUnitId, "clicked." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinInterstitialOnAdClicked", m_adUnitId);
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_interstitial_load_failed")
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.interstitial.state." + m_adUnitId, "load_failed");

        m_plugin.pythonCall("onApplovinInterstitialOnAdLoadFailed", m_adUnitId);

        this.retryLoadAd();
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_interstitial_display_failed")
            .addParameterString("placement", placement)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.interstitial.state." + m_adUnitId, "display_failed." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinInterstitialOnAdDisplayFailed", m_adUnitId);

        MengineUtils.performOnMainThread(() -> {
            MengineAppLovinInterstitial.this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_interstitial_revenue_paid")
            .addParameterString("placement", placement)
            .addParameterDouble("revenue", ad.getRevenue())
            .addParameterString("revenue_precision", ad.getRevenuePrecision())
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        m_plugin.pythonCall("onApplovinInterstitialOnAdRevenuePaid", m_adUnitId);
    }

    @Override
    public void onExpiredAdReloaded(@NonNull MaxAd adOld, @NonNull MaxAd adNew) {
        this.logMaxAd("onExpiredAdReloaded", adOld);

        this.buildAdEvent("mng_ad_interstitial_expired_reloaded")
            .addParameterJSON("old_ad", this.getMAAdParams(adOld))
            .addParameterJSON("new_ad", this.getMAAdParams(adNew))
            .log();
    }
}
