package org.Mengine.Plugin.AppLovin.AppOpenAd;

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
import com.applovin.mediation.ads.MaxAppOpenAd;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAnalyticsEventBuilder;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinAppOpenAdInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinBase;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinPluginInterface;

import java.util.Map;

public class MengineAppLovinAppOpenAd extends MengineAppLovinBase implements MengineAppLovinAppOpenAdInterface, MaxAdListener, MaxAdRequestListener, MaxAdExpirationListener, MaxAdRevenueListener, MaxAdReviewListener {
    public static final String METADATA_APPOPEN_PLACEMENT = "mengine.applovin.appopen.placement";
    public static final String METADATA_APPOPEN_ADUNITID = "mengine.applovin.appopen.adunitid";

    protected final String m_placement;

    protected MaxAppOpenAd m_appOpenAd;

    public MengineAppLovinAppOpenAd(@NonNull MengineAppLovinPluginInterface plugin) throws MengineServiceInvalidInitializeException {
        super(plugin, MaxAdFormat.APP_OPEN);

        String MengineAppLovinPlugin_AppOpen_AdUnitId = plugin.getMetaDataString(METADATA_APPOPEN_ADUNITID);

        if (MengineAppLovinPlugin_AppOpen_AdUnitId.isEmpty() == true) {
            this.invalidInitialize("meta %s is empty"
                , METADATA_APPOPEN_ADUNITID
            );
        }

        this.setAdUnitId(MengineAppLovinPlugin_AppOpen_AdUnitId);

        String MengineAppLovinPlugin_AppOpen_Placement = plugin.getMetaDataString(METADATA_APPOPEN_PLACEMENT);

        m_placement = MengineAppLovinPlugin_AppOpen_Placement;
    }

    protected MengineAnalyticsEventBuilder buildAppOpenAdEvent(@Size(min = 1L,max = 40L) String event) {
        MengineAnalyticsEventBuilder builder = this.buildAdEvent("mng_ad_appopen_" + event)
            .addParameterString("placement", m_placement)
            ;

        return builder;
    }

    protected void setAppOpenState(@NonNull String state) {
        this.setState("applovin.appopen.state." + m_adUnitId, state);
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) {
        super.onActivityCreate(activity);

        MaxAppOpenAd appOpenAd = new MaxAppOpenAd(m_adUnitId);

        appOpenAd.setListener( this );
        appOpenAd.setRequestListener( this );
        appOpenAd.setExpirationListener( this );
        appOpenAd.setRevenueListener( this );
        appOpenAd.setAdReviewListener( this );

        m_appOpenAd = appOpenAd;

        this.log("create");

        this.setAppOpenState("init");

        this.firstLoadAd((mediation, callback) -> {
            mediation.loadMediatorAppOpen(activity, m_plugin, m_appOpenAd, callback);
        });
    }

    @Override
    public void onActivityDestroy(@NonNull MengineActivity activity) {
        super.onActivityDestroy(activity);

        if (m_appOpenAd != null) {
            m_appOpenAd.destroy();
            m_appOpenAd = null;
        }
    }

    @Override
    public boolean canYouShowAppOpen(String placement) {
        if (m_appOpenAd == null) {
            return false;
        }

        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_appOpenAd.isReady();

        this.log("canYouShowAppOpen", Map.of("placement", placement, "ready", ready));

        if (ready == false) {
            this.buildAppOpenAdEvent("show")
                .addParameterBoolean("ready", false)
                .log();

            return false;
        }

        return true;
    }

    @Override
    public boolean showAppOpen(String placement) {
        if (m_appOpenAd == null) {
            return false;
        }

        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_appOpenAd.isReady();

        this.log("showAppOpen", Map.of("placement", placement, "ready", ready));

        this.buildAppOpenAdEvent("show")
            .addParameterBoolean("ready", ready)
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

        if (m_plugin.hasOption("applovin.appopen.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildAppOpenAdEvent("load")
            .log();

        this.setAppOpenState("load");

        try {
            m_appOpenAd.loadAd();
        } catch (final Exception e) {
            this.logError("loadAd", e);

            this.buildAppOpenAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.setAppOpenState("load_exception");

            this.retryLoadAd();
        }
    }

    @Override
    public void onAdRequestStarted(@NonNull String adUnitId) {
        this.log("onAdRequestStarted");

        this.buildAppOpenAdEvent("started")
            .log();

        this.setAppOpenState("request_started");
    }

    @Override
    public void onAdLoaded(@NonNull MaxAd ad) {
        this.logMaxAd("onAdLoaded", ad);

        this.buildAppOpenAdEvent("loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setAppOpenState("loaded." + ad.getNetworkName());

        m_requestAttempt = 0;
    }

    @Override
    public void onAdDisplayed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        String placement = ad.getPlacement();

        this.buildAppOpenAdEvent("displayed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setAppOpenState("displayed." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdHidden(@NonNull MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        String placement = ad.getPlacement();

        this.buildAppOpenAdEvent("hidden")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setAppOpenState("hidden." + placement + "." + ad.getNetworkName());

        MengineUtils.performOnMainThread(() -> {
            m_adResponse.onAdShowSuccess(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_APPOPEN, placement);

            this.loadAd();
        });
    }

    @Override
    public void onAdClicked(@NonNull MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        String placement = ad.getPlacement();

        this.buildAppOpenAdEvent("clicked")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setAppOpenState("clicked." + placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildAppOpenAdEvent("load_failed")
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterLong("error_code", errorCode)
            .log();

        this.setAppOpenState("load_failed." + errorCode);

        this.retryLoadAd();
    }

    @Override
    public void onAdDisplayFailed(@NonNull MaxAd ad, @NonNull MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildAppOpenAdEvent("display_failed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterLong("error_code", errorCode)
            .log();

        this.setAppOpenState("display_failed." + placement + "." + ad.getNetworkName() + "." + errorCode);

        MengineUtils.performOnMainThread(() -> {
            m_adResponse.onAdShowFailed(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_APPOPEN, placement, errorCode);

            this.loadAd();
        });
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        this.buildAppOpenAdEvent("revenue_paid")
            .addParameterDouble("revenue", ad.getRevenue())
            .addParameterString("revenue_precision", ad.getRevenuePrecision())
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.revenuePaid(ad);
    }

    @Override
    public void onExpiredAdReloaded(@NonNull MaxAd adOld, @NonNull MaxAd adNew) {
        this.logMaxAd("onExpiredAdReloaded", adOld);

        this.buildAppOpenAdEvent("expired_reloaded")
            .addParameterJSON("old_ad", this.getMAAdParams(adOld))
            .addParameterJSON("new_ad", this.getMAAdParams(adNew))
            .log();
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
