package org.Mengine.Plugin.AppLovin;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.ads.MaxInterstitialAd;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineUtils;

import java.util.concurrent.TimeUnit;

public class MengineAppLovinInterstitial extends MengineAppLovinBase implements MaxAdRequestListener, MaxAdListener, MaxAdRevenueListener {
    private MaxInterstitialAd m_interstitialAd;

    private int m_retryAttempt;

    private int m_enumeratorRequest;
    private int m_requestId;

    public MengineAppLovinInterstitial(MengineAppLovinPlugin plugin, String adUnitId) {
        super(plugin);

        m_retryAttempt = 0;
        m_enumeratorRequest = 0;
        m_requestId = 0;

        MengineActivity activity = m_plugin.getMengineActivity();

        MaxInterstitialAd interstitialAd = new MaxInterstitialAd(adUnitId, activity);

        interstitialAd.setRequestListener(this);
        interstitialAd.setListener(this);
        interstitialAd.setRevenueListener(this);

        m_interstitialAd = interstitialAd;

        m_plugin.logMessage("[Interstitial] create adUnitId: %s"
            , adUnitId
        );

        m_plugin.setState("applovin.interstitial.state." + m_interstitialAd.getAdUnitId(), "init");

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

        String adUnitId = m_interstitialAd.getAdUnitId();

        m_requestId = m_enumeratorRequest++;

        m_plugin.logInfo("[Interstitial] loadAd adUnitId: %s request: %d attempt: %d"
            , adUnitId
            , m_requestId
            , m_retryAttempt
        );

        this.buildEvent("mng_ad_interstitial_load")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("attempt", m_retryAttempt)
            .log();

        m_plugin.setState("applovin.interstitial.state." + adUnitId, "load");

        m_interstitialAd.loadAd();
    }

    public boolean canYouShowInterstitial(String placement) {
        if (m_interstitialAd == null) {
            return false;
        }

        String adUnitId = m_interstitialAd.getAdUnitId();

        boolean ready = m_interstitialAd.isReady();

        m_plugin.logMessage("[Interstitial] canYouShowInterstitial adUnitId: %s ready: %b request: %d attempt: %d"
            , adUnitId
            , ready
            , m_requestId
            , m_retryAttempt
        );

        if (ready == false) {
            this.buildEvent("mng_ad_interstitial_show")
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

    public boolean showInterstitial(String placement) {
        if (m_interstitialAd == null) {
            return false;
        }

        String adUnitId = m_interstitialAd.getAdUnitId();

        boolean ready = m_interstitialAd.isReady();

        m_plugin.logMessage("[Interstitial] showInterstitial adUnitId: %s ready: %b request: %d attempt: %d"
            , adUnitId
            , ready
            , m_requestId
            , m_retryAttempt
        );

        this.buildEvent("mng_ad_interstitial_show")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("attempt", m_retryAttempt)
            .addParameterBoolean("ready", ready)
            .log();

        if (ready == false) {
            return false;
        }

        m_interstitialAd.showAd(placement);

        return true;
    }

    @Override
    public void onAdRequestStarted(String adUnitId) {
        m_plugin.logInfo("[Interstitial] onAdRequestStarted adUnitId: %s request: %d attempt: %d"
            , adUnitId
            , m_requestId
            , m_retryAttempt
        );

        this.buildEvent("mng_ad_interstitial_request_started")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("attempt", m_retryAttempt)
            .log();

        m_plugin.setState("applovin.interstitial.state." + adUnitId, "request_started");

        m_plugin.pythonCall("onApplovinBannerOnAdRequestStarted", adUnitId);
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdLoaded", ad);

        String adUnitId = ad.getAdUnitId();

        this.buildEvent("mng_ad_interstitial_loaded")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("attempt", m_retryAttempt)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.interstitial.state." + adUnitId, "loaded." + ad.getNetworkName());

        m_retryAttempt = 0;

        m_plugin.pythonCall("onApplovinInterstitialOnAdLoaded", adUnitId);
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdDisplayed", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_interstitial_displayed")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.interstitial.state." + adUnitId, "displayed." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinInterstitialOnAdDisplayed", adUnitId);
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdHidden", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_interstitial_hidden")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.interstitial.state." + adUnitId, "hidden." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinInterstitialOnAdHidden", adUnitId);

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdClicked", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_interstitial_clicked")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.interstitial.state." + adUnitId, "clicked." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinInterstitialOnAdClicked", adUnitId);
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        this.logMaxError("Interstitial", "onAdLoadFailed", adUnitId, error);

        int errorCode = error.getCode();

        this.buildEvent("mng_ad_interstitial_load_failed")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("attempt", m_retryAttempt)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.interstitial.state." + adUnitId, "load_failed");

        m_plugin.pythonCall("onApplovinInterstitialOnAdLoadFailed", adUnitId);

        m_retryAttempt++;

        long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttempt)));

        MengineUtils.performOnMainThreadDelayed(() -> {
            this.loadAd();
        }, delayMillis);
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("Interstitial", "onAdDisplayFailed", ad.getAdUnitId(), error);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildEvent("mng_ad_interstitial_display_failed")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.interstitial.state." + adUnitId, "display_failed." + placement + "." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinInterstitialOnAdDisplayFailed", adUnitId);

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdRevenuePaid", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_interstitial_revenue_paid")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        m_plugin.pythonCall("onApplovinInterstitialOnAdRevenuePaid", adUnitId);
    }
}
