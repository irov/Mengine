package org.Mengine.Plugin.AppLovin;

import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.FrameLayout;
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
import com.applovin.sdk.AppLovinAdSize;
import com.applovin.sdk.AppLovinSdkUtils;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineUtils;

public class MengineAppLovinBanner extends MengineAppLovinBase implements MaxAdRequestListener, MaxAdViewAdListener, MaxAdRevenueListener, MaxAdReviewListener {
    private MaxAdView m_adView;

    private int m_enumeratorRequest;
    private int m_requestId;

    public MengineAppLovinBanner(MengineAppLovinPlugin plugin, String adUnitId, String placement) {
        super(plugin);

        m_enumeratorRequest = 0;
        m_requestId = 0;

        MengineActivity activity = plugin.getMengineActivity();

        MaxAdView adView = new MaxAdView(adUnitId, activity);

        adView.setPlacement(placement);

        adView.setRequestListener(this);
        adView.setListener(this);
        adView.setRevenueListener(this);
        adView.setAdReviewListener(this);

        int width = ViewGroup.LayoutParams.MATCH_PARENT;

        AppLovinSdkUtils.Size adaptiveSize = MaxAdFormat.BANNER.getAdaptiveSize(activity);

        int widthDp = adaptiveSize.getWidth();
        int heightDp = adaptiveSize.getHeight();
        int widthPx = AppLovinSdkUtils.dpToPx(activity, widthDp);
        int heightPx = AppLovinSdkUtils.dpToPx(activity, heightDp);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(width, heightPx);
        params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
        adView.setLayoutParams(params);
        adView.setBackgroundColor(android.graphics.Color.TRANSPARENT);
        adView.setExtraParameter("adaptive_banner", "true");

        adView.setVisibility(View.GONE);
        adView.stopAutoRefresh();

        ViewGroup viewGroup = MengineActivity.getContentViewGroup();
        viewGroup.addView(adView);

        m_adView = adView;

        m_plugin.logMessage("[Banner] create adUnitId: %s placement: %s size: [%d, %d]"
            , adUnitId
            , placement
            , widthPx
            , heightPx
        );

        m_plugin.setState("applovin.banner.state." + m_adView.getAdUnitId(), "init");

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
            m_adView.destroy();
            m_adView = null;
        }
    }

    public void loadAd() {
        if (m_adView == null) {
            return;
        }

        String adUnitId = m_adView.getAdUnitId();

        m_plugin.logMessage("[Banner] loadAd adUnitId: %s"
            , adUnitId
        );

        m_requestId = m_enumeratorRequest++;

        this.buildEvent("mng_ad_banner_load")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .log();

        m_plugin.setState("applovin.banner.state." + adUnitId, "load");

        try {
            m_adView.loadAd();
        } catch (Exception e) {
            m_plugin.logError("[Banner] loadAd adUnitId: %s exception: %s"
                , adUnitId
                , e.getMessage()
            );

            this.buildEvent("mng_ad_banner_load_exception")
                .addParameterString("ad_unit_id", adUnitId)
                .addParameterLong("request_id", m_requestId)
                .addParameterString("exception", e.getMessage())
                .log();

            m_plugin.setState("applovin.banner.state." + adUnitId, "load_exception");

            m_plugin.pythonCall("onApplovinBannerOnAdLoadException", adUnitId);
        }
    }

    public boolean bannerVisible(boolean show) {
        if(m_adView == null) {
            return false;
        }

        String adUnitId = m_adView.getAdUnitId();

        m_plugin.logMessage("[Rewarded] bannerVisible adUnitId: %s %s"
            , adUnitId
            , show == true ? "SHOW" : "HIDE"
        );

        if (show == true) {
            m_adView.startAutoRefresh();
            m_adView.setVisibility(View.VISIBLE);
        } else {
            m_adView.stopAutoRefresh();
            m_adView.setVisibility(View.GONE);
        }

        return true;
    }

    @Override
    public void onAdRequestStarted(String adUnitId) {
        m_plugin.logMessage("[Banner] onAdRequestStarted adUnitId: %s"
            , adUnitId
        );

        this.buildEvent("mng_ad_banner_request_started")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .log();

        m_plugin.setState("applovin.banner.state." + adUnitId, "request_started");

        m_plugin.pythonCall("onApplovinBannerOnAdRequestStarted", adUnitId);
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        this.logMaxAd("Banner","onAdLoaded", ad);

        String adUnitId = ad.getAdUnitId();

        this.buildEvent("mng_ad_banner_loaded")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + adUnitId, "loaded." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdLoaded", adUnitId);
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("Banner", "onAdDisplayed", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_banner_displayed")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + adUnitId, "displayed." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdDisplayed", adUnitId);
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("Banner", "onAdHidden", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_banner_hidden")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + adUnitId, "hidden." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdHidden", adUnitId);
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("Banner", "onAdClicked", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_banner_clicked")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + adUnitId, "clicked." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdClicked", adUnitId);
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        this.logMaxError("Banner", "onAdLoadFailed", adUnitId, error);

        int errorCode = error.getCode();

        this.buildEvent("mng_ad_banner_load_failed")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.banner.state." + adUnitId, "load_failed");

        m_plugin.pythonCall("onApplovinBannerOnAdLoadFailed", adUnitId);
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("Banner", "onAdDisplayFailed", ad.getAdUnitId(), error);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        int errorCode = error.getCode();

        this.buildEvent("mng_ad_banner_display_failed")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.setState("applovin.banner.state." + adUnitId, "display_failed." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdDisplayFailed", adUnitId);
    }

    @Override
    public void onAdExpanded(MaxAd ad) {
        this.logMaxAd("Banner", "onAdExpanded", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_banner_expanded")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + adUnitId, "expanded." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdExpanded", adUnitId);
    }

    @Override
    public void onAdCollapsed(MaxAd ad) {
        this.logMaxAd("Banner", "onAdCollapsed", ad);

        String adUnitId = ad.getAdUnitId();

        this.buildEvent("mng_ad_banner_collapsed")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + adUnitId, "collapsed." + ad.getNetworkName());

        m_plugin.pythonCall("onApplovinBannerOnAdCollapsed", adUnitId);
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("Banner", "onAdRevenuePaid", ad);

        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();

        this.buildEvent("mng_ad_banner_revenue_paid")
            .addParameterString("ad_unit_id", adUnitId)
            .addParameterString("placement", placement)
            .addParameterLong("request_id", m_requestId)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        m_plugin.pythonCall("onApplovinBannerOnAdRevenuePaid", adUnitId);
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        this.logMaxAd("Banner", "onCreativeIdGenerated", ad);

        m_plugin.logMessage("[Banner] onCreativeIdGenerated creativeId: %s"
                , creativeId
        );

        //ToDo
    }
}
