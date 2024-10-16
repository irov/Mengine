package org.Mengine.Plugin.AppLovin;

import android.graphics.Rect;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.graphics.Color;

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

    protected volatile boolean m_visible;
    protected volatile boolean m_loaded;

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
        adView.setBackgroundColor(Color.TRANSPARENT);
        adView.setExtraParameter("adaptive_banner", "true");

        adView.setVisibility(View.GONE);
        adView.stopAutoRefresh();

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
            m_adView.setListener(null);
            m_adView.setRequestListener(null);
            m_adView.setRevenueListener(null);
            m_adView.setAdReviewListener(null);

            m_adView.destroy();
            m_adView = null;
        }
    }

    public Rect getViewport() {
        if (m_adView == null) {
            m_plugin.logError("getViewport: adView is null");

            return null;
        }

        int left = m_adView.getLeft();
        int right = m_adView.getRight();
        int top = m_adView.getTop();
        int bottom = m_adView.getBottom();

        return new Rect(left, top, right, bottom);
    }

    public void loadAd() {
        if (m_adView == null) {
            return;
        }

        if (m_plugin.hasOption("applovin.banner.no_load") == true) {
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
        } catch (final Exception e) {
            m_plugin.logError("[Banner] loadAd adUnitId: %s exception: %s"
                , m_adUnitId
                , e.getMessage()
            );

            this.buildAdEvent("mng_ad_banner_load_exception")
                .addParameterString("placement", placement)
                .addParameterException("exception", e)
                .log();

            m_plugin.setState("applovin.banner.state." + m_adUnitId, "load_exception");

            m_plugin.pythonCall("onAppLovinBannerOnAdLoadException", m_adUnitId);
        }
    }

    protected void enableAdView(@NonNull MaxAdView adView) {
        if (adView.getVisibility() == View.VISIBLE) {
            return;
        }

        MengineActivity activity = m_plugin.getMengineActivity();

        ViewGroup viewGroup = activity.getContentViewGroup();
        viewGroup.addView(adView);

        adView.setVisibility(View.VISIBLE);

        if (m_plugin.hasOption("applovin.banner.no_load") == false) {
            adView.startAutoRefresh();
        }
    }

    protected void disableAdView(@NonNull MaxAdView adView) {
        if (adView.getVisibility() == View.GONE) {
            return;
        }

        adView.setVisibility(View.GONE);

        if (m_plugin.hasOption("applovin.banner.no_load") == false) {
            adView.stopAutoRefresh();
        }

        MengineActivity activity = m_plugin.getMengineActivity();

        ViewGroup viewGroup = activity.getContentViewGroup();
        viewGroup.removeView(adView);
    }

    public boolean bannerVisible(boolean show) {
        MaxAdView copy_adView = m_adView;

        m_plugin.runOnUiThread(() -> {
            if (m_visible == show) {
                return;
            }

            m_visible = show;

            MengineAppLovinBanner.this.log("bannerVisible", Map.of("show", show));

            MengineAppLovinNonetBannersInterface nonetBanners = m_plugin.getNonetBanners();

            if (show == true) {
                if (nonetBanners != null) {
                    if (m_loaded == true) {
                        nonetBanners.hide();

                        if (copy_adView != null) {
                            MengineAppLovinBanner.this.enableAdView(copy_adView);
                        }
                    } else {
                        nonetBanners.show();
                    }
                } else {
                    if (copy_adView != null) {
                        MengineAppLovinBanner.this.enableAdView(copy_adView);
                    }
                }
            } else {
                if (copy_adView != null) {
                    MengineAppLovinBanner.this.disableAdView(copy_adView);
                }

                if (nonetBanners != null) {
                    nonetBanners.hide();
                }
            }
        });

        return true;
    }

    @Override
    public void onAdRequestStarted(@NonNull String adUnitId) {
        this.increaseRequestId();

        this.log("onAdRequestStarted");

        this.buildAdEvent("mng_ad_banner_request_started")
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "request_started");

        m_plugin.pythonCall("onAppLovinBannerOnAdRequestStarted", m_adUnitId);
    }

    @Override
    public void onAdLoaded(@NonNull MaxAd ad) {
        m_loaded = true;

        this.logMaxAd("onAdLoaded", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_loaded")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_retryAttempt = 0;

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "loaded." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinBannerOnAdLoaded", m_adUnitId);

        MengineAppLovinNonetBannersInterface nonetBanners = m_plugin.getNonetBanners();

        if (m_visible == true) {
            if (nonetBanners != null) {
                MengineAppLovinBanner.this.enableAdView(m_adView);

                nonetBanners.hide();
            }
        }
    }

    @Override
    public void onAdDisplayed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_displayed")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "displayed." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinBannerOnAdDisplayed", m_adUnitId);
    }

    @Override
    public void onAdHidden(@NonNull MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_hidden")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "hidden." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinBannerOnAdHidden", m_adUnitId);
    }

    @Override
    public void onAdClicked(@NonNull MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_clicked")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "clicked." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinBannerOnAdClicked", m_adUnitId);
    }

    @Override
    public void onAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
        m_loaded = false;

        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildAdEvent("mng_ad_banner_load_failed")
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_retryAttempt++;

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "load_failed");

        m_plugin.pythonCall("onAppLovinBannerOnAdLoadFailed", m_adUnitId);

        MengineAppLovinNonetBannersInterface nonetBanners = m_plugin.getNonetBanners();

        if (m_visible == true) {
            if (nonetBanners != null) {
                MengineAppLovinBanner.this.disableAdView(m_adView);

                nonetBanners.show();
            }
        }
    }

    @Override
    public void onAdDisplayFailed(@NonNull MaxAd ad, @NonNull MaxError error) {
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

        m_plugin.pythonCall("onAppLovinBannerOnAdDisplayFailed", m_adUnitId);
    }

    @Override
    public void onAdExpanded(@NonNull MaxAd ad) {
        this.logMaxAd("onAdExpanded", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_expanded")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "expanded." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinBannerOnAdExpanded", m_adUnitId);
    }

    @Override
    public void onAdCollapsed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdCollapsed", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_collapsed")
            .addParameterString("placement", placement)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "collapsed." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinBannerOnAdCollapsed", m_adUnitId);
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        String placement = ad.getPlacement();

        this.buildAdEvent("mng_ad_banner_revenue_paid")
            .addParameterString("placement", placement)
            .addParameterDouble("revenue", ad.getRevenue())
            .addParameterString("revenue_precision", ad.getRevenuePrecision())
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        double revenue = ad.getRevenue();

        m_plugin.pythonCall("onAppLovinBannerOnAdRevenuePaid"
            , m_adUnitId
            , Map.of("revenue", revenue)
        );
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
