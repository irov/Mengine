package org.Mengine.Plugin.AppLovin;

import android.view.View;
import android.view.ViewGroup;
import android.graphics.Color;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.Size;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxAdReviewListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.nativeAds.MaxNativeAdListener;
import com.applovin.mediation.nativeAds.MaxNativeAdLoader;
import com.applovin.mediation.nativeAds.MaxNativeAdView;
import com.applovin.mediation.nativeAds.MaxNativeAdViewBinder;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAnalyticsEventBuilder;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineUtils;

import java.util.Map;

class MengineAppLovinNativeAd extends MengineAppLovinBase implements MaxAdRevenueListener, MaxAdReviewListener {
    protected final String m_placement;

    protected MaxNativeAdView m_adView;
    protected MaxNativeAdLoader m_adLoader;
    protected MaxAd m_cachedAd;

    protected volatile boolean m_visible = false;
    protected volatile boolean m_loaded = false;

    public MengineAppLovinNativeAd(MengineAppLovinPlugin plugin, String adUnitId, String placement) {
        super(plugin, adUnitId, MaxAdFormat.NATIVE);

        m_placement = placement;
    }

    protected MengineAnalyticsEventBuilder buildNativeAdEvent(@Size(min = 1L,max = 40L) String event) {
        MengineAnalyticsEventBuilder builder = this.buildAdEvent("mng_ad_native_" + event)
            .addParameterString("placement", m_placement);

        return builder;
    }

    protected void setNativeState(@NonNull String state) {
        this.setState("applovin.native.state." + m_adUnitId, state);
    }

    @Override
    public boolean isInitialized() {
        return m_adView != null;
    }

    @Override
    public void initialize(@NonNull MengineApplication application) {
        super.initialize(application);

        MaxNativeAdViewBinder binder = new MaxNativeAdViewBinder.Builder(R.layout.native_ad_view)
            .setTitleTextViewId(R.id.title_text_view)
            .setBodyTextViewId(R.id.body_text_view)
            .setStarRatingContentViewGroupId(R.id.star_rating_view)
            .setAdvertiserTextViewId(R.id.advertiser_text_view)
            .setIconImageViewId(R.id.icon_image_view)
            .setMediaContentViewGroupId(R.id.media_view_container)
            .setOptionsContentViewGroupId(R.id.options_view)
            .setCallToActionButtonId(R.id.cta_button)
            .build();

        MaxNativeAdView adView = new MaxNativeAdView(binder, application);

        adView.setBackgroundColor(Color.TRANSPARENT);

        if (m_visible == false) {
            adView.setVisibility(View.GONE);
        } else {
            adView.setVisibility(View.VISIBLE);
        }

        m_adView = adView;

        MaxNativeAdLoader adLoader = new MaxNativeAdLoader(m_adUnitId);

        adLoader.setPlacement(m_placement);

        adLoader.setNativeAdListener(new MaxNativeAdListener() {
            @Override
            public void onNativeAdLoaded(@Nullable MaxNativeAdView nativeAdView, @NonNull MaxAd nativeAd) {
                if (m_cachedAd != null) {
                    m_adLoader.destroy(m_cachedAd);
                    m_cachedAd = null;
                }

                MengineAppLovinNativeAd.this.logMaxAd("onNativeAdLoaded", nativeAd);

                MengineAppLovinNativeAd.this.buildNativeAdEvent("loaded")
                    .addParameterJSON("ad", MengineAppLovinNativeAd.this.getMAAdParams(nativeAd))
                    .log();

                m_requestAttempt = 0;

                m_loaded = true;

                m_cachedAd = nativeAd;

                MengineAppLovinNativeAd.this.setNativeState("loaded." + m_placement + "." + nativeAd.getNetworkName());
            }

            @Override
            public void onNativeAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
                MengineAppLovinNativeAd.this.logMaxError("onNativeAdLoadFailed", error);

                int errorCode = error.getCode();

                MengineAppLovinNativeAd.this.buildNativeAdEvent("load_failed")
                    .addParameterJSON("error", MengineAppLovinNativeAd.this.getMaxErrorParams(error))
                    .addParameterLong("error_code", errorCode)
                    .log();

                MengineAppLovinNativeAd.this.setNativeState("load_failed." + m_placement + "." + errorCode);

                MengineAppLovinNativeAd.this.retryLoadAd();
            }

            @Override
            public void onNativeAdClicked(@NonNull MaxAd ad) {
                MengineAppLovinNativeAd.this.logMaxAd("onNativeAdClicked", ad);

                MengineAppLovinNativeAd.this.buildNativeAdEvent("clicked")
                    .addParameterJSON("ad", MengineAppLovinNativeAd.this.getMAAdParams(ad))
                    .log();

                MengineAppLovinNativeAd.this.setNativeState("clicked." + m_placement + "." + ad.getNetworkName());
            }

            @Override
            public void onNativeAdExpired(@NonNull MaxAd ad) {
                MengineAppLovinNativeAd.this.logMaxAd("onNativeAdExpired", ad);

                MengineAppLovinNativeAd.this.buildNativeAdEvent("expired")
                    .addParameterJSON("ad", MengineAppLovinNativeAd.this.getMAAdParams(ad))
                    .log();

                m_loaded = false;

                MengineAppLovinNativeAd.this.setNativeState("expired." + m_placement + "." + ad.getNetworkName());

                MengineAppLovinNativeAd.this.retryLoadAd();
            }
        });

        adLoader.setRevenueListener(this);
        adLoader.setAdReviewListener(this);

        m_adLoader = adLoader;

        this.log("create", Map.of("placement", m_placement));

        this.setNativeState("init." + m_placement);

        MengineUtils.performOnMainThread(() -> {
            this.loadAd();
        });
    }

    @Override
    public void finalize(@NonNull MengineApplication application) {
        super.finalize(application);

        if (m_adLoader != null) {
            if (m_cachedAd != null) {
                m_adLoader.destroy(m_cachedAd);
                m_cachedAd = null;
            }

            m_adLoader.setNativeAdListener(null);
            m_adLoader.setRevenueListener(null);
            m_adLoader.setAdReviewListener(null);

            m_adLoader.destroy();
            m_adLoader = null;
        }

        m_adView = null;
    }

    public int getLeftMarginPx() {
        //ToDo

        return 0;
    }

    public int getTopMarginPx() {
        //ToDo

        return 0;
    }

    public int getWidthPx() {
        //ToDo

        return 0;
    }

    public int getHeightPx() {
        //ToDo

        return 0;
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

        if (m_plugin.hasOption("applovin.native.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildNativeAdEvent("load")
            .log();

        this.setNativeState("load");

        try {
            m_adLoader.loadAd(m_adView);
        } catch (final Exception e) {
            m_plugin.logError("[Native] loadAd adUnitId: %s exception: %s"
                , m_adUnitId
                , e.getMessage()
            );

            this.buildNativeAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.setNativeState("load_exception");

            this.retryLoadAd();
        }
    }

    protected void enableAdView(@NonNull MaxNativeAdView adView) {
        if (adView.getVisibility() == View.VISIBLE) {
            return;
        }

        if (m_plugin.hasOption("applovin.native.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        adView.setVisibility(View.VISIBLE);
        adView.requestLayout();
    }

    protected void disableAdView(@NonNull MaxNativeAdView adView) {
        if (adView.getVisibility() == View.GONE) {
            return;
        }

        if (m_plugin.hasOption("applovin.native.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        adView.setVisibility(View.GONE);
        adView.requestLayout();
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
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        this.buildNativeAdEvent("revenue_paid")
            .addParameterDouble("revenue", ad.getRevenue())
            .addParameterString("revenue_precision", ad.getRevenuePrecision())
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_plugin.onEventRevenuePaid(ad);

        double revenue = ad.getRevenue();

        m_adResponse.onAdRevenuePaid(MengineAdMediation.ADMEDIATION_APPLOVINMAX, MengineAdFormat.ADFORMAT_NATIVE, m_placement, revenue);
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
