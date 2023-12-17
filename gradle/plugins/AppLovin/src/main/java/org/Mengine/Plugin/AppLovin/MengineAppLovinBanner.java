package org.Mengine.Plugin.AppLovin;

import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxAdReviewListener;
import com.applovin.mediation.MaxAdViewAdListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.ads.MaxAdView;
import com.applovin.sdk.AppLovinSdkUtils;

import org.Mengine.Base.MengineActivity;

import java.util.Map;

public class MengineAppLovinBanner extends MengineAppLovinBase implements MaxAdRequestListener, MaxAdViewAdListener, MaxAdRevenueListener, MaxAdReviewListener {
    protected MaxAdView m_adView;

    protected boolean m_visible;
    protected boolean m_loaded;

    public MengineAppLovinBanner(MengineAppLovinPlugin plugin, String adUnitId, String placement) {
        super(MaxAdFormat.BANNER, plugin, adUnitId);

        m_visible = false;
        m_loaded = false;

        MengineActivity activity = plugin.getMengineActivity();

        MaxAdView adView = new MaxAdView(adUnitId, activity);

        adView.setPlacement(placement);

        adView.setListener(this);
        adView.setRequestListener(this);
        adView.setRevenueListener(this);
        adView.setAdReviewListener(this);

        AppLovinSdkUtils.Size adaptiveSize = MaxAdFormat.BANNER.getAdaptiveSize(activity);

        int widthDp = adaptiveSize.getWidth();
        int heightDp = adaptiveSize.getHeight();
        int widthPx = AppLovinSdkUtils.dpToPx(activity, widthDp);
        int heightPx = AppLovinSdkUtils.dpToPx(activity, heightDp);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, heightPx);
        params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
        adView.setLayoutParams(params);
        adView.setBackgroundColor(android.graphics.Color.TRANSPARENT);
        adView.setExtraParameter("adaptive_banner", "true");

        adView.stopAutoRefresh();
        adView.setVisibility(View.GONE);

        ViewGroup viewGroup = MengineActivity.getContentViewGroup();
        viewGroup.addView(adView);

        m_adView = adView;

        m_plugin.logMessage("[%s] create adUnitId: %s placement: %s size: [%d, %d]"
            , m_adFormat.getLabel()
            , adUnitId
            , placement
            , widthPx
            , heightPx
        );

        m_plugin.setState("applovin.banner.state." + adUnitId, "init");

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            mediationAmazon.initializeMediatorBanner(activity, plugin, adView, new MengineAppLovinMediationLoadAdCallback() {
                @Override
                public void onLoadAd() {
                    MengineAppLovinBanner.this.loadAd();
                }
            });
        } else {
            // Load the ad
            this.loadAd();
        }
    }

    @Override
    public void destroy() {
        super.destroy();

        if (m_adView != null) {
            m_adView.setRequestListener(null);
            m_adView.setListener(null);
            m_adView.setRevenueListener(null);
            m_adView.setAdReviewListener(null);

            m_adView.destroy();
            m_adView = null;
        }
    }

    public void loadAd() {
        if (m_adView == null) {
            return;
        }

        this.log("loadAd");

        String placement = m_adView.getPlacement();

        this.buildAdEvent("mng_ad_banner_load")
            .addParameterString("placement", placement)
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "load");

        try {
            m_adView.loadAd();
        } catch (Exception e) {
            m_plugin.logError("[Banner] loadAd adUnitId: %s exception: %s"
                , m_adUnitId
                , e.getMessage()
            );

            this.buildAdEvent("mng_ad_banner_load_exception")
                .addParameterString("placement", placement)
                .addParameterString("exception", e.getMessage())
                .log();

            m_plugin.setState("applovin.banner.state." + m_adUnitId, "load_exception");

            m_plugin.pythonCall("onApplovinBannerOnAdLoadException", m_adUnitId);
        }
    }

    public boolean bannerVisible(boolean show) {
        if(m_adView == null) {
            return false;
        }

        m_visible = show;

        MaxAdView copy_adView = m_adView;

        m_plugin.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                MengineAppLovinBanner.this.log("bannerVisible", Map.of("show", show));

                MengineAppLovinNonetBanners nonetBanners = m_plugin.getNonetBanners();

                if (show == true) {
                    nonetBanners.hide();

                    copy_adView.startAutoRefresh();
                    copy_adView.setVisibility(View.VISIBLE);
                } else {
                    copy_adView.stopAutoRefresh();
                    copy_adView.setVisibility(View.GONE);

                    nonetBanners.show();
                }
            }
        });

        if (m_loaded == false) {
            MengineAppLovinNonetBanners nonetBanners = m_plugin.getNonetBanners();
            nonetBanners.show();
        }

        return true;
    }

    @Override
    public void onAdRequestStarted(@NonNull String adUnitId) {
        this.increaseRequestId();

        this.log("onAdRequestStarted");

        this.buildAdEvent("mng_ad_banner_request_started")
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "request_started");

        m_plugin.pythonCall("onApplovinBannerOnAdRequestStarted", m_adUnitId);
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        m_loaded = true;

        this.logMaxAd("onAdLoaded", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_loaded")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_retryAttempt = 0;

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "loaded." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdLoaded", m_adUnitId);

        if (m_visible == true) {
            m_adView.setVisibility(View.VISIBLE);

            MengineAppLovinNonetBanners nonetBanners = m_plugin.getNonetBanners();
            nonetBanners.hide();
        }
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_displayed")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "displayed." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdDisplayed", m_adUnitId);
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_hidden")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "hidden." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdHidden", m_adUnitId);
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_clicked")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "clicked." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdClicked", m_adUnitId);
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        m_loaded = false;

        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_banner_load_failed")
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_retryAttempt++;

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "load_failed");

        m_plugin.pythonCall("onApplovinBannerOnAdLoadFailed", m_adUnitId);

        if (m_visible == true) {
            m_adView.setVisibility(View.GONE);

            MengineAppLovinNonetBanners nonetBanners = m_plugin.getNonetBanners();
            nonetBanners.show();
        }
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_banner_display_failed")
            .addParameterString("placement", placement)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "display_failed." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdDisplayFailed", m_adUnitId);
    }

    @Override
    public void onAdExpanded(MaxAd ad) {
        this.logMaxAd("onAdExpanded", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_expanded")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "expanded." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdExpanded", m_adUnitId);
    }

    @Override
    public void onAdCollapsed(MaxAd ad) {
        this.logMaxAd("onAdCollapsed", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_collapsed")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "collapsed." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdCollapsed", m_adUnitId);
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_revenue_paid")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        m_plugin.pythonCall("onApplovinBannerOnAdRevenuePaid", m_adUnitId);
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
