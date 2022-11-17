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
    private MengineAppLovinPlugin m_plugin;

    private MaxAdView m_adView;

    public MengineAppLovinBanner(MengineAppLovinPlugin plugin) throws Exception {
        super(plugin);

        MengineActivity activity = plugin.getActivity();

        String AppLovin_BannerAdUnitId = activity.getConfigValue("AppLovinPlugin", "BannerAdUnitId", "");

        if (AppLovin_BannerAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] BannerAdUnitId");
        }

        MaxAdView adView = new MaxAdView(AppLovin_BannerAdUnitId, activity);

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
            adView.loadAd();
        }

        adView.setVisibility(View.GONE);

        m_adView = adView;
    }

    public void destroy() {
        super.destroy();

        if (m_adView != null) {
            m_adView.destroy();
            m_adView = null;
        }
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

        m_plugin.pythonCall("onApplovinBannerOnAdRequestStarted");
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        this.logMaxAd( "Banner","onAdLoaded", ad);

        m_plugin.pythonCall("onApplovinBannerOnAdLoaded");
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("Banner", "onAdDisplayed", ad);

        m_plugin.pythonCall("onApplovinBannerOnAdDisplayed");
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("Banner", "onAdHidden", ad);

        m_plugin.pythonCall("onApplovinBannerOnAdHidden");
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("Banner", "onAdClicked", ad);

        m_plugin.pythonCall("onApplovinBannerOnAdClicked");
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        this.logMaxError("Banner", "onAdLoadFailed", error);

        m_plugin.pythonCall("onApplovinBannerOnAdLoadFailed");
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("Banner", "onAdDisplayFailed", error);

        m_plugin.pythonCall("onApplovinBannerOnAdDisplayFailed");
    }

    @Override
    public void onAdExpanded(MaxAd ad) {
        this.logMaxAd("Banner", "onAdExpanded", ad);

        m_plugin.pythonCall("onApplovinBannerOnAdExpanded");
    }

    @Override
    public void onAdCollapsed(MaxAd ad) {
        this.logMaxAd("Banner", "onAdCollapsed", ad);

        m_plugin.pythonCall("onApplovinBannerOnAdCollapsed");
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("Banner", "onAdRevenuePaid", ad);

        m_plugin.pythonCall("onApplovinBannerOnAdRevenuePaid");

        m_plugin.eventRevenuePaid(ad);
    }
}
