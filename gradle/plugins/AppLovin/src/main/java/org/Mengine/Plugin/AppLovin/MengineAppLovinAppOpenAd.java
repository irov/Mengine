package org.Mengine.Plugin.AppLovin;

import androidx.annotation.NonNull;

import androidx.lifecycle.DefaultLifecycleObserver;
import androidx.lifecycle.LifecycleOwner;
import androidx.lifecycle.ProcessLifecycleOwner;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdExpirationListener;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxAdReviewListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.ads.MaxAppOpenAd;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

public class MengineAppLovinAppOpenAd extends MengineAppLovinBase implements DefaultLifecycleObserver, MaxAdListener, MaxAdRequestListener, MaxAdExpirationListener, MaxAdRevenueListener, MaxAdReviewListener {
    protected final String m_placement;

    protected MaxAppOpenAd m_appOpenAd;

    public MengineAppLovinAppOpenAd(@NonNull MengineAppLovinPlugin plugin, String adUnitId, String placement) {
        super(plugin, adUnitId, MaxAdFormat.APP_OPEN);

        m_placement = placement;
    }

    @Override
    public void initialize(@NonNull MengineApplication application) {
        MaxAppOpenAd appOpenAd = new MaxAppOpenAd(m_adUnitId, application);

        appOpenAd.setListener( this );
        appOpenAd.setRequestListener( this );
        appOpenAd.setExpirationListener( this );
        appOpenAd.setRevenueListener( this );
        appOpenAd.setAdReviewListener( this );

        m_appOpenAd = appOpenAd;

        ProcessLifecycleOwner.get().getLifecycle().addObserver(this);

        this.loadAd();
    }

    @Override
    public void finalize(@NonNull MengineApplication application) {
        ProcessLifecycleOwner.get().getLifecycle().removeObserver( this);

        m_appOpenAd.destroy();
        m_appOpenAd = null;
    }

    private boolean showAdIfReady(String placement) {
        boolean ready = m_appOpenAd.isReady();

        this.log("showAdIfReady", Map.of("placement", placement, "ready", ready));

        this.buildAdEvent("mng_ad_appopen_show")
            .addParameterString("placement", placement)
            .addParameterBoolean("ready", false)
            .log();

        if (ready == false) {
            return false;
        }

        m_appOpenAd.showAd(placement);

        return true;
    }

    @Override
    public void loadAd() {
        if (m_appOpenAd == null) {
            return;
        }

        if (m_plugin.hasOption("applovin.appopen.no_load") == true || m_plugin.hasOption("applovin.ad.no_load") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildAdEvent("mng_ad_appopen_load")
            .log();

        m_plugin.setState("applovin.appopen.state." + m_adUnitId, "load");

        try {
            m_appOpenAd.loadAd();
        } catch (final Exception e) {
            this.logError("loadAd", e);

            this.buildAdEvent("mng_ad_appopen_load_exception")
                .addParameterException("exception", e)
                .log();

            m_plugin.setState("applovin.appopen.state." + m_adUnitId, "load_exception");

            this.retryLoadAd();
        }
    }

    @Override
    public void onStart(@NonNull LifecycleOwner owner) {
        this.showAdIfReady(m_placement);
    }

    @Override
    public void onAdRequestStarted(@NonNull String adUnitId) {
        this.log("onAdRequestStarted");

        this.buildAdEvent("mng_ad_appopen_request_started")
            .log();

        m_plugin.setState("applovin.appopen.state." + m_adUnitId, "request_started");
    }

    @Override
    public void onAdLoaded(@NonNull MaxAd ad) {
        this.logMaxAd("onAdLoaded", ad);

        this.buildAdEvent("mng_ad_appopen_loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.appopen.state." + m_adUnitId, "loaded." + ad.getNetworkName());

        m_requestAttempt = 0;
    }

    @Override
    public void onAdDisplayed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_appopen_displayed")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.appopen.state." + m_adUnitId, "displayed." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdHidden(@NonNull MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_appopen_hidden")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.appopen.state." + m_adUnitId, "hidden." + placement + "." + ad.getNetworkName());

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
    }

    @Override
    public void onAdClicked(@NonNull MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_appopen_clicked")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.appopen.state." + m_adUnitId, "clicked." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_appopen_load_failed")
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.appopen.state." + m_adUnitId, "load_failed." + errorCode);

        this.retryLoadAd();
    }

    @Override
    public void onAdDisplayFailed(@NonNull MaxAd ad, @NonNull MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_appopen_display_failed")
            .addParameterString("placement", placement)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.appopen.state." + m_adUnitId, "display_failed." + placement + "." + ad.getNetworkName() + "." + errorCode);

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_appopen_revenue_paid")
            .addParameterString("placement", placement)
            .addParameterDouble("revenue", ad.getRevenue())
            .addParameterString("revenue_precision", ad.getRevenuePrecision())
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        double revenue = ad.getRevenue();

        m_plugin.pythonCall("onAppLovinAppOpenRevenuePaid", Map.of("placement", placement, "revenue", revenue));
    }

    @Override
    public void onExpiredAdReloaded(@NonNull MaxAd adOld, @NonNull MaxAd adNew) {
        this.logMaxAd("onExpiredAdReloaded", adOld);

        this.buildAdEvent("mng_ad_appopen_expired_reloaded")
            .addParameterJSON("old_ad", this.getMAAdParams(adOld))
            .addParameterJSON("new_ad", this.getMAAdParams(adNew))
            .log();
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
