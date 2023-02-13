package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxAdViewAdListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.ads.MaxAdView;

import com.applovin.sdk.AppLovinAdSize;
import com.applovin.sdk.AppLovinSdkUtils;

import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.FrameLayout;

public class MengineAppLovinBanner extends MengineAppLovinBase implements MaxAdRequestListener, MaxAdViewAdListener, MaxAdRevenueListener {
    private MaxAdView m_adView;

    private int m_enumeratorRequest;
    private int m_requestId;

    public MengineAppLovinBanner(MengineAppLovinPlugin plugin, String adUnitId) throws Exception {
        super(plugin);

        m_enumeratorRequest = 0;
        m_requestId = 0;

        MengineActivity activity = plugin.getActivity();

        MaxAdView adView = new MaxAdView(adUnitId, activity);

        adView.setRequestListener(this);
        adView.setListener(this);
        adView.setRevenueListener(this);

        int width = ViewGroup.LayoutParams.MATCH_PARENT;

        boolean tablet = AppLovinSdkUtils.isTablet(activity);
        AppLovinAdSize size = tablet ? AppLovinAdSize.LEADER : AppLovinAdSize.BANNER;
        int tablet_size_height = size.getHeight();
        int heightPx = AppLovinSdkUtils.dpToPx(activity, tablet_size_height);

        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(width, heightPx);
        adView.setLayoutParams(params);

        Window window = activity.getWindow();
        View view = window.getDecorView();
        int window_view_height = view.getHeight();

        adView.setTranslationY(window_view_height - heightPx);

        ViewGroup rootView = view.findViewById(android.R.id.content);
        rootView.addView(adView);

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            mediationAmazon.initializeMediatorBanner(activity, adView);
        } else {
            // Load the ad
            this.loadAd();
        }

        adView.setVisibility(View.GONE);

        m_adView = adView;
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
        m_requestId = m_enumeratorRequest++;

        this.buildEvent("ad_banner_load")
            .addParameterInteger("request_id", m_requestId)
            .log();

        m_adView.loadAd();
    }

    public void bannerVisible(boolean show) {
        if (show) {
            m_adView.startAutoRefresh();
            m_adView.setVisibility(View.VISIBLE);
        } else {
            m_adView.stopAutoRefresh();
            m_adView.setVisibility(View.GONE);
        }
    }

    @Override
    public void onAdRequestStarted(String adUnitId) {
        m_plugin.logInfo("[Banner] onAdRequestStarted %s"
            , adUnitId
        );

        this.buildEvent("ad_banner_request_started")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("unit_id", adUnitId)
            .log();

        m_plugin.pythonCall("onApplovinBannerOnAdRequestStarted");
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        this.logMaxAd( "Banner","onAdLoaded", ad);

        this.buildEvent("ad_banner_loaded")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinBannerOnAdLoaded");
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("Banner", "onAdDisplayed", ad);

        this.buildEvent("ad_banner_displayed")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinBannerOnAdDisplayed");
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("Banner", "onAdHidden", ad);

        this.buildEvent("ad_banner_hidden")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinBannerOnAdHidden");
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("Banner", "onAdClicked", ad);

        this.buildEvent("ad_banner_clicked")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinBannerOnAdClicked");
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        this.logMaxError("Banner", "onAdLoadFailed", error);

        this.buildEvent("ad_banner_load_failed")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("unit_id", adUnitId)
            .addParameterString("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.pythonCall("onApplovinBannerOnAdLoadFailed");
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("Banner", "onAdDisplayFailed", error);

        this.buildEvent("ad_banner_display_failed")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .addParameterString("error", this.getMaxErrorParams(error))
            .log();

        m_plugin.pythonCall("onApplovinBannerOnAdDisplayFailed");
    }

    @Override
    public void onAdExpanded(MaxAd ad) {
        this.logMaxAd("Banner", "onAdExpanded", ad);

        this.buildEvent("ad_banner_expanded")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinBannerOnAdExpanded");
    }

    @Override
    public void onAdCollapsed(MaxAd ad) {
        this.logMaxAd("Banner", "onAdCollapsed", ad);

        this.buildEvent("ad_banner_collapsed")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinBannerOnAdCollapsed");
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("Banner", "onAdRevenuePaid", ad);

        this.buildEvent("ad_revenue_paid")
            .addParameterInteger("request_id", m_requestId)
            .addParameterString("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.pythonCall("onApplovinBannerOnAdRevenuePaid");

        m_plugin.onEventRevenuePaid(ad);
    }
}
