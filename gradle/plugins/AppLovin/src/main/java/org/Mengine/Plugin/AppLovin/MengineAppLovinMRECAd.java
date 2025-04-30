package org.Mengine.Plugin.AppLovin;

import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
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
import org.Mengine.Base.MengineUtils;

import java.util.Map;

class MengineAppLovinMRECAd extends MengineAppLovinBase implements MaxAdRequestListener, MaxAdViewAdListener, MaxAdRevenueListener, MaxAdReviewListener {
    protected final String m_placement;

    protected MaxAdView m_adView;

    protected volatile boolean m_visible = false;
    protected volatile boolean m_loaded = false;

    public MengineAppLovinMRECAd(MengineAppLovinPlugin plugin, String adUnitId, String placement) {
        super(plugin, adUnitId, MaxAdFormat.MREC);

        m_placement = placement;
    }

    protected MengineAnalyticsEventBuilder buildMRECAdEvent(@Size(min = 1L,max = 40L) String event) {
        MengineAnalyticsEventBuilder builder = this.buildAdEvent("mng_ad_mrec_" + event)
            .addParameterString("placement", m_placement);

        return builder;
    }

    protected void setMRECState(@NonNull String state) {
        this.setState("applovin.mrec.state." + m_adUnitId, state);
    }

    protected AppLovinSdkUtils.Size getMRECSize() {
        AppLovinSdkUtils.Size size = MaxAdFormat.MREC.getSize();

        return size;
    }

    @Override
    public boolean isInitialized() {
        return m_adView != null;
    }

    @Override
    public void initialize(@NonNull MengineApplication application) {
        super.initialize(application);

        MaxAdViewConfiguration config = MaxAdViewConfiguration.builder()
            .setAdaptiveType(MaxAdViewConfiguration.AdaptiveType.NONE)
            .build();

        MaxAdView adView = new MaxAdView(m_adUnitId, MaxAdFormat.MREC, config);

        adView.setPlacement(m_placement);

        adView.setListener(this);
        adView.setRequestListener(this);
        adView.setRevenueListener(this);
        adView.setAdReviewListener(this);

        AppLovinSdkUtils.Size size = this.getMRECSize();

        int widthDp = size.getWidth();
        int heightDp = size.getHeight();

        int widthPx = AppLovinSdkUtils.dpToPx(application, widthDp);
        int heightPx = AppLovinSdkUtils.dpToPx(application, heightDp);

        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(widthPx, heightPx);

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

        this.setMRECState("init." + m_placement);

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
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

    public int getLeftMarginPx() {
        if (m_adView == null) {
            return 0;
        }

        FrameLayout.LayoutParams params = (FrameLayout.LayoutParams)m_adView.getLayoutParams();

        int leftMarginPx = params.leftMargin;

        return leftMarginPx;
    }

    public int getTopMarginPx() {
        if (m_adView == null) {
            return 0;
        }

        FrameLayout.LayoutParams params = (FrameLayout.LayoutParams)m_adView.getLayoutParams();

        int topMarginPx = params.topMargin;

        return topMarginPx;
    }

    public int getWidthPx() {
        MengineApplication application = m_plugin.getMengineApplication();

        AppLovinSdkUtils.Size size = this.getMRECSize();

        int widthDp = size.getWidth();
        int widthPx = AppLovinSdkUtils.dpToPx(application, widthDp);

        return widthPx;
    }

    public int getHeightPx() {
        MengineApplication application = m_plugin.getMengineApplication();

        AppLovinSdkUtils.Size size = this.getMRECSize();

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

        if (m_plugin.hasOption("applovin.mrec.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildMRECAdEvent("load")
            .log();

        this.setMRECState("load");

        try {
            m_adView.loadAd();
        } catch (final Exception e) {
            m_plugin.logError("[MREC] loadAd adUnitId: %s exception: %s"
                , m_adUnitId
                , e.getMessage()
            );

            this.buildMRECAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.setMRECState("load_exception");

            this.retryLoadAd();
        }
    }

    protected void enableAdView(@NonNull MaxAdView adView) {
        if (adView.getVisibility() == View.VISIBLE) {
            return;
        }

        if (m_plugin.hasOption("applovin.mrec.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
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

        if (m_plugin.hasOption("applovin.mrec.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        adView.setVisibility(View.GONE);
        adView.requestLayout();

        adView.setExtraParameter( "allow_pause_auto_refresh_immediately", "true" );
        adView.stopAutoRefresh();
    }

    private void updateVisible() {
        this.log("updateVisible", Map.of("show", m_visible));

        if (m_visible == true) {
            if (m_adView != null) {
                this.enableAdView(m_adView);
            }
        } else {
            if (m_adView != null) {
                this.disableAdView(m_adView);
            }
        }
    }

    public boolean canYouShow() {
        return m_loaded;
    }

    public void show(int leftMargin, int topMargin) {
        m_plugin.runOnUiThread(() -> {
            if (m_visible == true) {
                return;
            }

            m_visible = true;

            FrameLayout.LayoutParams params = (FrameLayout.LayoutParams)m_adView.getLayoutParams();
            params.leftMargin = leftMargin;
            params.topMargin = topMargin;

            m_adView.setLayoutParams(params);
            m_adView.requestLayout();

            this.updateVisible();
        });
    }

    public void hide() {
        m_plugin.runOnUiThread(() -> {
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

        this.buildMRECAdEvent("request_started")
            .log();

        this.setMRECState("request_started." + m_placement);
    }

    @Override
    public void onAdLoaded(@NonNull MaxAd ad) {
        m_loaded = true;

        this.logMaxAd("onAdLoaded", ad);

        this.buildMRECAdEvent("loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_requestAttempt = 0;

        this.setMRECState("loaded." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdDisplayed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        this.buildMRECAdEvent("displayed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setMRECState("displayed." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdHidden(@NonNull MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        this.buildMRECAdEvent("hidden")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setMRECState("hidden." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdClicked(@NonNull MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        this.buildMRECAdEvent("clicked")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setMRECState("clicked." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildMRECAdEvent("load_failed")
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        m_requestAttempt++;

        this.setMRECState("load_failed." + m_placement + "." + errorCode);
    }

    @Override
    public void onAdDisplayFailed(@NonNull MaxAd ad, @NonNull MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        int errorCode = error.getCode();

        this.buildMRECAdEvent("display_failed")
            .addParameterLong("error_code", errorCode)
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .log();

        this.setMRECState("display_failed." + m_placement + "." + ad.getNetworkName() + "." + errorCode);
    }

    @Override
    public void onAdExpanded(@NonNull MaxAd ad) {
        this.logMaxAd("onAdExpanded", ad);

        this.buildMRECAdEvent("expanded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setMRECState("expanded." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdCollapsed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdCollapsed", ad);

        this.buildMRECAdEvent("collapsed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setMRECState("collapsed." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        this.buildMRECAdEvent("revenue_paid")
            .addParameterDouble("revenue", ad.getRevenue())
            .addParameterString("revenue_precision", ad.getRevenuePrecision())
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        double revenue = ad.getRevenue();

        m_adResponse.onAdRevenuePaid(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_MREC, m_placement, revenue);
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
