package org.Mengine.Plugin.AppLovin;

import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.graphics.Color;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxAdReviewListener;
import com.applovin.mediation.MaxAdViewAdListener;
import com.applovin.mediation.MaxAdViewConfiguration;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.ads.MaxAdView;
import com.applovin.sdk.AppLovinSdkUtils;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAnalyticsEventBuilder;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

class MengineAppLovinBannerAd extends MengineAppLovinBase implements MaxAdRequestListener, MaxAdViewAdListener, MaxAdRevenueListener, MaxAdReviewListener {
    protected final String m_placement;

    protected MaxAdView m_adView;

    protected volatile boolean m_visible = false;
    protected volatile boolean m_loaded = false;

    public MengineAppLovinBannerAd(MengineAppLovinPlugin plugin, String adUnitId, String placement) {
        super(plugin, adUnitId, MaxAdFormat.BANNER);

        m_placement = placement;
    }

    protected AppLovinSdkUtils.Size getBannerSize(@NonNull MengineApplication application) {
        AppLovinSdkUtils.Size size = MaxAdFormat.BANNER.getAdaptiveSize(application);

        return size;
    }

    protected MengineAnalyticsEventBuilder buildBannerAdEvent(@Size(min = 1L,max = 40L) String event) {
        MengineAnalyticsEventBuilder builder = this.buildAdEvent("mng_ad_banner_" + event)
            .addParameterString("placement", m_placement);

        return builder;
    }

    protected void setBannerState(@NonNull String state) {
        this.setState("applovin.banner.state." + m_adUnitId, state);
    }

    @Override
    public boolean isInitialized() {
        return m_adView != null;
    }

    @Override
    public void initialize(@NonNull MengineApplication application) {
        super.initialize(application);

        MaxAdViewConfiguration adConfig = MaxAdViewConfiguration.builder()
            .setAdaptiveType(MaxAdViewConfiguration.AdaptiveType.ANCHORED)
            .build();

        MaxAdView adView = new MaxAdView(m_adUnitId, adConfig);

        adView.setPlacement(m_placement);

        adView.setListener(this);
        adView.setRequestListener(this);
        adView.setRevenueListener(this);
        adView.setAdReviewListener(this);

        AppLovinSdkUtils.Size size = this.getBannerSize(application);

        int widthDp = size.getWidth();
        int heightDp = size.getHeight();

        int widthPx = AppLovinSdkUtils.dpToPx(application, widthDp);
        int heightPx = AppLovinSdkUtils.dpToPx(application, heightDp);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, heightPx);
        params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);

        adView.setLayoutParams(params);

        adView.setBackgroundColor(Color.TRANSPARENT);

        if (m_visible == false) {
            adView.setVisibility(View.GONE);

            adView.setExtraParameter( "allow_pause_auto_refresh_immediately", "true" );
            adView.stopAutoRefresh();
        } else {
            adView.setVisibility(View.VISIBLE);

            adView.startAutoRefresh();
        }

        m_adView = adView;

        this.log("create", Map.of("placement", m_placement, "width", widthDp, "height", heightDp));

        this.setBannerState("init." + m_placement);

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            try {
                mediationAmazon.initializeMediatorBanner(application, m_plugin, adView, () -> {
                    MengineUtils.performOnMainThread(() -> {
                        this.loadAd();
                    });
                });
            } catch (final MengineServiceInvalidInitializeException e) {
                m_plugin.logError("initializeMediatorBanner exception: %s"
                    , e.getMessage()
                );

                MengineUtils.performOnMainThread(() -> {
                    this.loadAd();
                });
            }
        } else {
            // Load the ad
            MengineUtils.performOnMainThread(() -> {
                this.loadAd();
            });
        }
    }

    @Override
    public void finalize(@NonNull MengineApplication application) {
        super.finalize(application);

        if (m_adView != null) {
            m_adView.setListener(null);
            m_adView.setRequestListener(null);
            m_adView.setRevenueListener(null);
            m_adView.setAdReviewListener(null);

            m_adView.destroy();
            m_adView = null;
        }
    }

    public int getWidthPx() {
        MengineApplication application = m_plugin.getMengineApplication();

        AppLovinSdkUtils.Size size = this.getBannerSize(application);

        int widthDp = size.getWidth();
        int widthPx = AppLovinSdkUtils.dpToPx(application, widthDp);

        return widthPx;
    }

    public int getHeightPx() {
        MengineApplication application = m_plugin.getMengineApplication();

        AppLovinSdkUtils.Size size = this.getBannerSize(application);

        int heightDp = size.getHeight();
        int heightPx = AppLovinSdkUtils.dpToPx(application, heightDp);

        return heightPx;
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) {
        ViewGroup viewGroup = activity.getContentViewGroup();
        if (viewGroup.indexOfChild(m_adView) == -1) {
            viewGroup.addView(m_adView);
        }
    }

    @Override
    public void onActivityDestroy(@NonNull MengineActivity activity) {
        ViewGroup viewGroup = activity.getContentViewGroup();
        if (viewGroup.indexOfChild(m_adView) != -1) {
            viewGroup.removeView(m_adView);
        }
    }

    @Override
    public void loadAd() {
        if (m_adView == null) {
            return;
        }

        if (m_plugin.hasOption("applovin.banner.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildBannerAdEvent("load")
            .log();

        this.setBannerState("load");

        try {
            m_adView.loadAd();
        } catch (final Exception e) {
            m_plugin.logError("[Banner] loadAd adUnitId: %s exception: %s"
                , m_adUnitId
                , e.getMessage()
            );

            this.buildBannerAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.setBannerState("load_exception");

            this.retryLoadAd();
        }
    }

    protected void enableAdView(@NonNull MaxAdView adView) {
        if (adView.getVisibility() == View.VISIBLE) {
            return;
        }

        if (m_plugin.hasOption("applovin.banner.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        adView.setVisibility(View.VISIBLE);
        adView.requestLayout();

        adView.startAutoRefresh();
    }

    protected void disableAdView(@NonNull MaxAdView adView) {
        if (adView.getVisibility() == View.GONE) {
            return;
        }

        if (m_plugin.hasOption("applovin.banner.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        adView.setVisibility(View.GONE);
        adView.requestLayout();

        adView.setExtraParameter( "allow_pause_auto_refresh_immediately", "true" );
        adView.stopAutoRefresh();
    }

    private void updateVisible() {
        this.log("updateVisible", Map.of("show", m_visible));

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

    public boolean canYouShow() {
        return m_loaded;
    }

    public void show() {
        m_plugin.runOnUiThread("show", () -> {
            if (m_visible == true) {
                return;
            }

            m_visible = true;

            this.updateVisible();
        });
    }

    public void hide() {
        m_plugin.runOnUiThread("hide", () -> {
            if (m_visible == false) {
                return;
            }

            m_visible = false;

            this.updateVisible();
        });
    }

    @Override
    public void onAdRequestStarted(@NonNull String adUnitId) {
        this.increaseRequestId();

        this.log("onAdRequestStarted");

        this.buildBannerAdEvent("request_started")
            .log();

        this.setBannerState("request_started." + m_placement);
    }

    @Override
    public void onAdLoaded(@NonNull MaxAd ad) {
        m_loaded = true;

        this.logMaxAd("onAdLoaded", ad);

        this.buildBannerAdEvent("loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_requestAttempt = 0;

        this.setBannerState("loaded." + m_placement + "." + ad.getNetworkName());

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

        this.buildBannerAdEvent("displayed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setBannerState("displayed." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdHidden(@NonNull MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        this.buildBannerAdEvent("hidden")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setBannerState("hidden." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdClicked(@NonNull MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        this.buildBannerAdEvent("clicked")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setBannerState("clicked." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildBannerAdEvent("load_failed")
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterLong("error_code", errorCode)
            .log();

        m_requestAttempt++;

        this.setBannerState("load_failed." + m_placement + "." + errorCode);

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

        this.buildBannerAdEvent("display_failed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterLong("error_code", errorCode)
            .log();

        this.setBannerState("display_failed." + m_placement + "." + ad.getNetworkName() + "." + errorCode);
    }

    @Override
    public void onAdExpanded(@NonNull MaxAd ad) {
        this.logMaxAd("onAdExpanded", ad);

        this.buildBannerAdEvent("expanded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setBannerState("expanded." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdCollapsed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdCollapsed", ad);

        this.buildBannerAdEvent("collapsed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setBannerState("collapsed." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        this.buildBannerAdEvent("revenue_paid")
            .addParameterDouble("revenue", ad.getRevenue())
            .addParameterString("revenue_precision", ad.getRevenuePrecision())
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        double revenue = ad.getRevenue();

        m_adResponse.onAdRevenuePaid(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_BANNER, m_placement, revenue);
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
