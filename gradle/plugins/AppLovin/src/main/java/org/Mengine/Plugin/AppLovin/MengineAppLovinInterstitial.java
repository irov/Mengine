package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineUtils;
import org.Mengine.Base.MengineActivity;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.ads.MaxInterstitialAd;

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

        MengineActivity activity = m_plugin.getActivity();

        MaxInterstitialAd interstitialAd = new MaxInterstitialAd(adUnitId, activity);

        interstitialAd.setRequestListener(this);
        interstitialAd.setListener(this);
        interstitialAd.setRevenueListener(this);

        m_interstitialAd = interstitialAd;

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

        m_requestId = m_enumeratorRequest++;

        m_plugin.logInfo("[Interstitial] loadAd request: %d attempt: %d"
            , m_requestId
            , m_retryAttempt
        );

        this.buildEvent("ad_interstitial_load")
            .addParameterInteger("request_id", m_requestId)
            .addParameterInteger("attempt", m_retryAttempt)
            .log();

        m_interstitialAd.loadAd();
    }

    public boolean canYouShowInterstitial() {
        boolean ready = m_interstitialAd.isReady();

        m_plugin.logMessage("[Interstitial] canYouShowInterstitial ready: %b request: %d attempt: %d"
            , ready
            , m_requestId
            , m_retryAttempt
        );

        if (ready == false) {
            this.buildEvent("ad_interstitial_show")
                .addParameterInteger("request_id", m_requestId)
                .addParameterInteger("attempt", m_retryAttempt)
                .addParameterBoolean("ready", false)
                .log();

            return false;
        }

        return true;
    }

    public boolean showInterstitial() {
        boolean ready = m_interstitialAd.isReady();

        m_plugin.logMessage("[Interstitial] showInterstitial ready: %b request: %d attempt: %d"
            , ready
            , m_requestId
            , m_retryAttempt
        );

        this.buildEvent("ad_interstitial_show")
            .addParameterInteger("request_id", m_requestId)
            .addParameterInteger("attempt", m_retryAttempt)
            .addParameterBoolean("ready", ready)
            .log();

        if (ready == false) {
            return false;
        }

        m_interstitialAd.showAd();

        return true;
    }

    @Override
    public void onAdRequestStarted(String adUnitId) {
        m_plugin.logInfo("[Interstitial] onAdRequestStarted: %s request: %d attempt: %d"
            , adUnitId
            , m_requestId
            , m_retryAttempt
        );

        this.buildEvent("ad_interstitial_request_started")
            .addParameterInteger("request_id", m_requestId)
            .addParameterInteger("attempt", m_retryAttempt)
            .addParameterString("unit_id", adUnitId)
            .log();

        m_plugin.pythonCall("onApplovinBannerOnAdRequestStarted");
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdLoaded", ad);

        this.buildEvent("ad_interstitial_loaded")
            .addParameterInteger("request_id", m_requestId)
            .addParameterInteger("attempt", m_retryAttempt)
            .addParameterString( "ad", this.getMAAdParams(ad))
            .log();

        m_retryAttempt = 0;

        m_plugin.pythonCall("onApplovinInterstitialOnAdLoaded");
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdDisplayed", ad);

        this.buildEvent("ad_interstitial_displayed")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString( "ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinInterstitialOnAdDisplayed");
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdHidden", ad);

        this.buildEvent("ad_interstitial_hidden")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString( "ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinInterstitialOnAdHidden");

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdClicked", ad);

        this.buildEvent("ad_interstitial_clicked")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinInterstitialOnAdClicked");
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        this.logMaxError("Interstitial", "onAdLoadFailed", error);

        this.buildEvent("ad_interstitial_load_failed")
            .addParameterInteger("request_id", m_requestId)
            .addParameterInteger("attempt", m_retryAttempt)
            .addParameterString("unit_id", adUnitId)
            .addParameterString("error", this.getMaxErrorParams(error))
            .addParameterInteger("error_code", error.getCode())
            .log();

        m_plugin.pythonCall("onApplovinInterstitialOnAdLoadFailed");

        m_retryAttempt++;

        long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttempt)));

        MengineUtils.performOnMainThreadDelayed(() -> {
            this.loadAd();
        }, delayMillis);
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("Interstitial", "onAdDisplayFailed", error);

        this.buildEvent("ad_interstitial_display_failed")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .addParameterString("error", this.getMaxErrorParams(error))
            .addParameterInteger("error_code", error.getCode())
            .log();

        m_plugin.pythonCall("onApplovinInterstitialOnAdDisplayFailed");

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdRevenuePaid", ad);

        this.buildEvent("ad_interstitial_revenue_paid")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        m_plugin.pythonCall("onApplovinInterstitialOnAdRevenuePaid");
    }
}
