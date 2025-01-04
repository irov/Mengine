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
import org.Mengine.Base.MengineAnalyticsEventBuilder;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

public class MengineAppLovinBanner extends MengineAppLovinBase implements MaxAdRequestListener, MaxAdViewAdListener, MaxAdRevenueListener, MaxAdReviewListener {
    protected final String m_placement;
    protected final boolean m_adaptive;

    protected MaxAdView m_adView;

    protected volatile boolean m_visible;
    protected volatile boolean m_loaded;

    public MengineAppLovinBanner(MengineAppLovinPlugin plugin, String adUnitId, String placement, boolean adaptive) {
        super(MaxAdFormat.BANNER, plugin, adUnitId);

        m_placement = placement;
        m_adaptive = adaptive;

        m_visible = false;
        m_loaded = false;
    }

    protected AppLovinSdkUtils.Size getBannerSize(MengineActivity activity) {
        AppLovinSdkUtils.Size size;

        if (m_adaptive == true) {
            size = MaxAdFormat.BANNER.getAdaptiveSize(activity);
        } else {
            boolean isTablet = AppLovinSdkUtils.isTablet(activity);

            size = (isTablet == true) ? MaxAdFormat.LEADER.getSize() : MaxAdFormat.BANNER.getSize();
        }

        return size;
    }

    @Override
    public void onCreate(MengineActivity activity) {
        super.onCreate(activity);

        MaxAdView adView = new MaxAdView(m_adUnitId, activity);

        adView.setPlacement(m_placement);

        adView.setListener(this);
        adView.setRequestListener(this);
        adView.setRevenueListener(this);
        adView.setAdReviewListener(this);

        AppLovinSdkUtils.Size size = this.getBannerSize(activity);

        int widthDp = size.getWidth();
        int heightDp = size.getHeight();
        int widthPx = AppLovinSdkUtils.dpToPx(activity, widthDp);
        int heightPx = AppLovinSdkUtils.dpToPx(activity, heightDp);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, heightPx);
        params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);

        adView.setLayoutParams(params);

        if (m_adaptive == true) {
            adView.setExtraParameter("adaptive_banner", "true");
        }

        adView.setBackgroundColor(Color.TRANSPARENT);

        adView.setVisibility(View.GONE);

        if (m_visible == false) {
            adView.setExtraParameter( "allow_pause_auto_refresh_immediately", "true" );
            adView.stopAutoRefresh();
        } else {
            adView.startAutoRefresh();
        }

        m_adView = adView;

        this.log("create");

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "init");

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            mediationAmazon.initializeMediatorBanner(activity, m_plugin, adView, () -> {
                this.initAd();
            });
        } else {
            // Load the ad
            MengineUtils.performOnMainThread(() -> {
                this.initAd();
            });
        }
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        super.onDestroy(activity);

        if (m_adView != null) {
            ViewGroup viewGroup = activity.getContentViewGroup();
            viewGroup.removeView(m_adView);

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

    public int getHeight() {
        MengineActivity activity = m_plugin.getMengineActivity();

        AppLovinSdkUtils.Size size = this.getBannerSize(activity);

        int heightDp = size.getHeight();
        int heightPx = AppLovinSdkUtils.dpToPx(activity, heightDp);

        return heightPx;
    }

    protected MengineAnalyticsEventBuilder buildBannerAdEvent(String event) {
        MengineAnalyticsEventBuilder builder = this.buildAdEvent(event)
            .addParameterString("placement", m_placement)
            .addParameterBoolean("adaptive", m_adaptive);

        return builder;
    }

    protected void initAd() {
        if (m_adView == null) {
            return;
        }

        MengineActivity activity = m_plugin.getMengineActivity();

        ViewGroup viewGroup = activity.getContentViewGroup();
        viewGroup.addView(m_adView);

        this.loadAd();
    }

    public void loadAd() {
        if (m_adView == null) {
            return;
        }

        if (m_plugin.hasOption("applovin.banner.no_load") == true || m_plugin.hasOption("applovin.ad.no_load") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildBannerAdEvent("mng_ad_banner_load")
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "load");

        try {
            m_adView.loadAd();
        } catch (final Exception e) {
            m_plugin.logError("[Banner] loadAd adUnitId: %s exception: %s"
                , m_adUnitId
                , e.getMessage()
            );

            this.buildBannerAdEvent("mng_ad_banner_load_exception")
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

        adView.setVisibility(View.VISIBLE);

        if (m_plugin.hasOption("applovin.banner.no_load") == true || m_plugin.hasOption("applovin.ad.no_load") == true) {
            return;
        }

        adView.startAutoRefresh();
    }

    protected void disableAdView(@NonNull MaxAdView adView) {
        if (adView.getVisibility() == View.GONE) {
            return;
        }

        adView.setVisibility(View.GONE);

        if (m_plugin.hasOption("applovin.banner.no_load") == true || m_plugin.hasOption("applovin.ad.no_load") == true) {
            return;
        }

        adView.setExtraParameter( "allow_pause_auto_refresh_immediately", "true" );
        adView.stopAutoRefresh();
    }

    private void updateVisible() {
        this.log("bannerVisible", Map.of("show", m_visible));

        MengineAppLovinNonetBannersInterface nonetBanners = m_plugin.getNonetBanners();

        if (m_visible == true) {
            if (nonetBanners != null) {
                if (m_loaded == true) {
                    nonetBanners.hide();

                    if (m_adView != null) {
                        this.enableAdView(m_adView);
                    }
                } else {
                    nonetBanners.show();
                }
            } else {
                if (m_adView != null) {
                    this.enableAdView(m_adView);
                }
            }
        } else {
            if (m_adView != null) {
                this.disableAdView(m_adView);
            }

            if (nonetBanners != null) {
                nonetBanners.hide();
            }
        }
    }

    public boolean bannerVisible(boolean show) {
        m_plugin.runOnUiThread(() -> {
            if (m_visible == show) {
                return;
            }

            m_visible = show;

            this.updateVisible();
        });

        return true;
    }

    @Override
    public void onAdRequestStarted(@NonNull String adUnitId) {
        this.increaseRequestId();

        this.log("onAdRequestStarted");

        this.buildBannerAdEvent("mng_ad_banner_request_started")
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "request_started");

        m_plugin.pythonCall("onAppLovinBannerOnAdRequestStarted", m_adUnitId);
    }

    @Override
    public void onAdLoaded(@NonNull MaxAd ad) {
        m_loaded = true;

        this.logMaxAd("onAdLoaded", ad);

        this.buildBannerAdEvent("mng_ad_banner_loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_requestAttempt = 0;

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "loaded." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinBannerOnAdLoaded", m_adUnitId);

        MengineAppLovinNonetBannersInterface nonetBanners = m_plugin.getNonetBanners();

        if (m_visible == true) {
            if (nonetBanners != null) {
                this.enableAdView(m_adView);

                nonetBanners.hide();
            }
        }
    }

    @Override
    public void onAdDisplayed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        this.buildBannerAdEvent("mng_ad_banner_displayed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "displayed." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinBannerOnAdDisplayed", m_adUnitId);
    }

    @Override
    public void onAdHidden(@NonNull MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        this.buildBannerAdEvent("mng_ad_banner_hidden")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "hidden." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinBannerOnAdHidden", m_adUnitId);
    }

    @Override
    public void onAdClicked(@NonNull MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        this.buildBannerAdEvent("mng_ad_banner_clicked")
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

        this.buildBannerAdEvent("mng_ad_banner_load_failed")
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_requestAttempt++;

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "load_failed");

        m_plugin.pythonCall("onAppLovinBannerOnAdLoadFailed", m_adUnitId);

        MengineAppLovinNonetBannersInterface nonetBanners = m_plugin.getNonetBanners();

        if (m_visible == true) {
            if (nonetBanners != null) {
                this.disableAdView(m_adView);

                nonetBanners.show();
            }
        }
    }

    @Override
    public void onAdDisplayFailed(@NonNull MaxAd ad, @NonNull MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        int errorCode = error.getCode();

        this.buildBannerAdEvent("mng_ad_banner_display_failed")
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

        this.buildBannerAdEvent("mng_ad_banner_expanded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "expanded." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinBannerOnAdExpanded", m_adUnitId);
    }

    @Override
    public void onAdCollapsed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdCollapsed", ad);

        this.buildBannerAdEvent("mng_ad_banner_collapsed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.setState("applovin.banner.state." + m_adUnitId, "collapsed." + ad.getNetworkName());

        m_plugin.pythonCall("onAppLovinBannerOnAdCollapsed", m_adUnitId);
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        this.buildBannerAdEvent("mng_ad_banner_revenue_paid")
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
